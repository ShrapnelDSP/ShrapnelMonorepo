#include "noise_gate.h"
#include "abstract_dsp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TAG "noise_gate"

typedef enum { INIT, CLOSED, ATTACK, OPEN, RELEASE } state_t;

static state_t state;

static float *gain_buffer;
static float *filter_buffer;
static size_t buffer_size;

static float threshold;
static float threshold_low;

static int attack_samples;
static int hold_samples;
static int release_samples;

static float sample_rate;

static dspal_iir_t envelope_detect_filter;

int gate_init(void)
{
    envelope_detect_filter = dspal_iir_create(2);
    if(envelope_detect_filter == NULL)
    {
        return -1;
    }

    gate_reset();
    return 0;
}

// TODO a bunch of state is stored as static variables in functions and can't
// be reset
void gate_reset(void)
{
    state = INIT;

    memset(gain_buffer, 0, sizeof(*gain_buffer)*buffer_size);
    memset(filter_buffer, 0, sizeof(*filter_buffer)*buffer_size);
}

/** \brief Set the buffer size in number of samples */
// TODO what to do when allocation fails?
void gate_set_buffer_size(size_t a_buffer_size)
{
    if(gain_buffer)
    {
        free(gain_buffer);
        gain_buffer = NULL;
    }

    gain_buffer = calloc(a_buffer_size, sizeof(*gain_buffer));
    if(gain_buffer == NULL)
    {
        return;
    }

    if(filter_buffer)
    {
        free(filter_buffer);
        filter_buffer = NULL;
    }

    filter_buffer = calloc(a_buffer_size, sizeof(*filter_buffer));
    if(filter_buffer == NULL)
    {
        return;
    }

    buffer_size = a_buffer_size;
}

void gate_set_sample_rate(float a_sample_rate)
{
    float coeffs[5];
    dspal_biquad_design_lowpass(coeffs, 10.f/a_sample_rate, M_SQRT1_2);
    dspal_iir_set_coeffs(envelope_detect_filter, coeffs, 2);

    sample_rate = a_sample_rate;
}

static float db_to_ratio(float db)
{
    return powf(10.f, db/20.f);
}

void gate_set_threshold(float a_threshold, float a_hysteresis)
{
    assert(a_hysteresis >= 0.f);

    threshold = db_to_ratio(a_threshold);
    threshold_low = threshold * db_to_ratio(-1.f * a_hysteresis);

    assert(threshold_low <= threshold);
}

void gate_set_attack(float a_attack)
{
    attack_samples = sample_rate * a_attack / 1000.f;
}

void gate_set_hold(float a_hold)
{
    hold_samples = sample_rate * a_hold / 1000.f;
}

void gate_set_release(float a_release)
{
    release_samples = sample_rate * a_release / 1000.f;
}

void gate_analyse(const float *buf, size_t sample_count)
{
    assert(sample_count <= buffer_size);

    static float gain = 0;
    static state_t next_state = INIT;
    static int hold_count = 0;

    // rectify the signal
    for(size_t i = 0; i < sample_count; i++)
    {
        filter_buffer[i] = fabsf(buf[i]);
    }

    //low pass filter
    dspal_iir_process(envelope_detect_filter, filter_buffer, filter_buffer, sample_count);

    // work out the gain
    for(size_t i = 0; i < sample_count; i++)
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
                if(filter_buffer[i] > threshold_low)
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
                if(filter_buffer[i] > threshold_low)
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

void gate_process(float *buf, size_t sample_count)
{
    dspal_multiply(gain_buffer, buf, buf, sample_count);
}
