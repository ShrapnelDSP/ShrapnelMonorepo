#include "noise_gate.h"
#include "dsps_biquad.h"
#include "dsps_biquad_gen.h"
#include "dsps_mul.h"
#include <math.h>
#include "esp_log.h"

#define TAG "noise_gate"

typedef enum { INIT, CLOSED, ATTACK, OPEN, RELEASE } state_t;

static state_t state;
static float *gain_buffer;
static float *filter_buffer;

static int attack_samples;
static int release_samples;
static int hold_samples;
static int buf_size;

static float threshold;
static float hysteresis;

static float coeffs[5];
static float delay[3];

esp_err_t gate_init(size_t a_buf_size, float a_threshold, float
        a_hysteresis, float attack_ms, float release_ms, float hold_ms, float
        samplerate)
{
    gain_buffer = malloc(sizeof(float) * a_buf_size);
    if(gain_buffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to malloc gain_buffer");
        return ESP_FAIL;
    }

    filter_buffer = malloc(sizeof(float) * a_buf_size);
    if(gain_buffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to malloc filter_buffer");
        return ESP_FAIL;
    }

    //The low pass filter used for envelope detection
    dsps_biquad_gen_lpf_f32(coeffs, 10.f/samplerate, M_SQRT1_2);

    for(int i = 0; i < 5; i++)
    {
        ESP_LOGI(TAG, "filter coeffs[%d] = %f", i, coeffs[i]);
    }

    for(int i = 0; i < 3; i++)
    {
        delay[i] = 0.f;
    }

    threshold = a_threshold;
    hysteresis = a_hysteresis;

    attack_samples = samplerate * attack_ms / 1000.f;
    release_samples = samplerate * release_ms / 1000.f;
    hold_samples = samplerate * hold_ms / 1000.f;
    buf_size = a_buf_size;

    state = INIT;

    return ESP_OK;
}

void gate_set_threshold(float a_threshold)
{
    threshold = a_threshold;
}

void gate_analyse(const float *buf)
{
    static float gain = 0;
    static state_t next_state = INIT;
    static int hold_count = 0;

    // rectify the signal
    for(int i = 0; i < buf_size; i++)
    {
        filter_buffer[i] = fabsf(buf[i]);
    }

    //low pass filter
    dsps_biquad_f32_ae32(filter_buffer, filter_buffer, buf_size, coeffs, delay);

    // work out the gain
    for(int i = 0; i < buf_size; i++)
    {
        switch(state)
        {
            case INIT:
                //don't need to init hold_count, it will be initialised when
                //leaving the attack state
                gain = 0;
                next_state = CLOSED;
                break;
            case CLOSED:
                if(filter_buffer[i] > threshold)
                {
                    next_state = ATTACK;
                }
                else
                {
                    next_state = CLOSED;
                }
                break;
            case ATTACK:
                if(filter_buffer[i] > threshold)
                {
                    gain = gain + 1.f/attack_samples;;
                    if(gain > 1)
                    {
                        gain = 1;
                        hold_count = 0;
                        next_state = OPEN;
                    }
                }
                else
                {
                    next_state = RELEASE;
                }
                break;
            case OPEN:
                if(filter_buffer[i] > (threshold - hysteresis))
                {
                    hold_count = 0;
                    next_state = OPEN;
                }
                else
                {
                    hold_count = hold_count + 1;
                    if(hold_count > hold_samples)
                    {
                        next_state = RELEASE;
                    }
                    else
                    {
                        next_state = OPEN;
                    }
                }
                break;
            case RELEASE:
                if(filter_buffer[i] > (threshold - hysteresis))
                {
                    next_state = ATTACK;
                }
                else
                {
                    gain = gain - 1.f/release_samples;
                    if(gain < 0)
                    {
                        gain = 0;
                        next_state = CLOSED;
                    }
                }
                break;
            default:
                ESP_LOGE(TAG, "invalid state %d", state);
                next_state = INIT;
                break;
        }
            
        state = next_state;
        gain_buffer[i] = gain;
    }
}

void gate_process(float *buf)
{
    dsps_mul_f32_ae32(gain_buffer, buf, buf, buf_size, 1, 1, 1);
}
