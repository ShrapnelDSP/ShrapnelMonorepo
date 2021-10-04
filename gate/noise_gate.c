#include "noise_gate.h"
#include "abstract_dsp.h"
#include <math.h>
#include <stdlib.h>

#define TAG "noise_gate"

typedef enum { INIT, CLOSED, ATTACK, OPEN, RELEASE } state_t;

static state_t state;
static float *gain_buffer;
static float *filter_buffer;

static int attack_samples;
static int release_samples;
static int hold_samples;
static size_t buf_size;

static float threshold;
static float hysteresis;

static dspal_iir_t envelope_detect_filter;

int gate_init(size_t a_buf_size, float a_threshold, float
        a_hysteresis, float attack_ms, float release_ms, float hold_ms, float
        samplerate)
{
    gain_buffer = calloc(a_buf_size, sizeof(*gain_buffer));
    if(gain_buffer == NULL)
    {
        return -1;
    }

    filter_buffer = calloc(a_buf_size, sizeof(*filter_buffer));
    if(gain_buffer == NULL)
    {
        return -1;
    }

    envelope_detect_filter = dspal_iir_create(2);
    if(envelope_detect_filter == NULL)
    {
        return -1;
    }

    float coeffs[5];
    dspal_biquad_design_lowpass(coeffs, 10.f/samplerate, M_SQRT1_2);
    dspal_iir_set_coeffs(envelope_detect_filter, coeffs, 2);

    threshold = a_threshold;
    hysteresis = a_hysteresis;

    attack_samples = samplerate * attack_ms / 1000.f;
    release_samples = samplerate * release_ms / 1000.f;
    hold_samples = samplerate * hold_ms / 1000.f;
    buf_size = a_buf_size;

    state = INIT;

    return 0;
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
    for(size_t i = 0; i < buf_size; i++)
    {
        filter_buffer[i] = fabsf(buf[i]);
    }

    //low pass filter
    dspal_iir_process(envelope_detect_filter, filter_buffer, filter_buffer, buf_size);

    // work out the gain
    for(size_t i = 0; i < buf_size; i++)
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
                    gain = gain + 1.f/attack_samples;
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
                next_state = INIT;
                break;
        }

        state = next_state;
        gain_buffer[i] = gain;
    }
}

void gate_process(float *buf)
{
    dspal_multiply(gain_buffer, buf, buf, buf_size);
}
