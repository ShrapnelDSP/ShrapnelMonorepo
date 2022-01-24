#include "i2s.h"
#include "process.h"
#include "dsps_biquad.h"
#include "float_convert.h"
#include "dsps_fir.h"
#include "dsps_mulc.h"
#include "dsps_mul.h"
#include "math.h"
#include "iir.h"
#include "fmv.h"
#include "input_filter.h"
#include "audio_events.h"
#include "noise_gate.h"
#include "profiling.h"
#include "chorus.h"

#include "esp_log.h"
#define TAG "i2s_process"

#include "driver/gpio.h"

#include <assert.h>
#include <stdlib.h>

extern float pedal_gain;
extern float amp_gain;
extern float volume;
extern gpio_num_t g_profiling_gpio;
static float fbuf[DMA_BUF_SIZE];

#include "speaker_coeffs.h"
static float fir_delay_line[sizeof(fir_coeff)/sizeof(fir_coeff[0])];
static fir_f32_t fir;

static shrapnel::effect::Chorus *chorus;

#define EQ_GAIN (0.5)
//these approximate the pre EQ from a Boss HM-2 pedal
static float coeff[3][5] = {
    { 1.016224171805848*EQ_GAIN, -1.996244766867333*EQ_GAIN, 0.980170456681741*EQ_GAIN, 
        -1.996244766867333, 0.996394628487589 },
    { 1.048098514125369*EQ_GAIN, -1.946884930731663*EQ_GAIN, 0.914902628855116*EQ_GAIN,
        -1.946884930731663, 0.963001142980485 },
    { 1.260798129602192*EQ_GAIN, -1.896438187817481*EQ_GAIN, 0.674002337997260*EQ_GAIN,
        -1.896438187817481, 0.934800467599452}
};

static float delay_line[3][2];

static float waveshape(float x)
{
#if 0
    float y;
    if (x < -1.f) {
        y = -3.0f / 4.0f *
            (1.f - powf(1.f - (1 - 0.032847f), 12.f) +
             1.f / 3.f * (1 - 0.032847f)) +
            0.01f;
    }
    else if (x >= -1.f && x < -0.08905f) {
        y = -3.0f / 4.0f *
            (1.f - powf(1.f - (fabs(x) - 0.032847f), 12.f) +
             1.f / 3.f * (fabs(x) - 0.032847f)) +
            0.01f;
    } else if (x >= -0.08905f && x < 0.320018f) {
        y = -6.153f * powf(x, 2.f) + 3.9375f * x;
    } else {
        y = 0.630035f;
    }
    return y;
#else
    return tanhf(x);
#endif
}

void process_samples(int32_t *buf, size_t buf_len)
{
    assert(g_profiling_gpio != -1 && "I2S task has not been initialised");

    gpio_set_level(g_profiling_gpio, 1);
    profiling_start();
    /* Copy samples, skipping every second one. These correspond to the left
     * channel of the ADC and DAC, which are not used here 
     * XXX not sure why the right channel is not at index 0 */
    for(int i = 1; i < buf_len; i+=2)
    {
        fbuf[i/2] = buf[i]/(float)INT32_MAX;
    }
    profiling_mark_stage(0);

    gate_analyse(fbuf);
    profiling_mark_stage(1);
    dsps_mulc_f32_ae32(fbuf, fbuf, sizeof(fbuf)/sizeof(fbuf[0]), pedal_gain, 1, 1);
    profiling_mark_stage(2);
    filter_pedal_input_process(fbuf, buf_len/2);
    profiling_mark_stage(3);

    for(int i = 1; i < buf_len; i+=2)
    {
        fbuf[i/2] = waveshape(fbuf[i/2]);
    }
    profiling_mark_stage(4);

    /* HM-2 EQ filters */
    for(int i = 0; i < 3; i++)
    {
        dsps_biquad_f32_ae32(fbuf, fbuf, buf_len/2, coeff[i], delay_line[i]);
    }
    profiling_mark_stage(5);

    filter_amp_input_process(fbuf, buf_len/2);
    profiling_mark_stage(6);

    dsps_mulc_f32_ae32(fbuf, fbuf, buf_len/2, amp_gain, 1, 1);
    profiling_mark_stage(7);

    for(int i = 1; i < buf_len; i+=2)
    {
        fbuf[i/2] = waveshape(fbuf[i/2]);
    }
    profiling_mark_stage(8);

    fmv_process(fbuf, buf_len/2);
    profiling_mark_stage(9);
    gate_process(fbuf);
    profiling_mark_stage(10);
    filter_final_process(fbuf, buf_len/2);
    profiling_mark_stage(11);

    /* speaker IR */
    dsps_fir_f32_ae32(&fir, fbuf, fbuf, buf_len/2);
    profiling_mark_stage(12);

    chorus->set_modulation_rate_hz(1.f);
    chorus->set_modulation_depth(.5f);
    chorus->set_modulation_mix(.8f);
    chorus->process(fbuf, buf_len/2);

    dsps_mulc_f32_ae32(fbuf, fbuf, buf_len/2, volume, 1, 1);
    profiling_mark_stage(13);

    for(int i = 1; i < buf_len; i+=2)
    {
        if(fbuf[i/2] > 1.f || fbuf[i/2] < -1.f)
        {
            xEventGroupSetBits(g_audio_event_group, AUDIO_EVENT_OUTPUT_CLIPPED);
        }

        if(fbuf[i] == NAN)
        {
            ESP_LOGE(TAG, "Not a number");
        } 

        buf[i] = float_to_int32(fbuf[i/2]);
    }

    profiling_mark_stage(14);
    profiling_stop();
    gpio_set_level(g_profiling_gpio, 0);
}

esp_err_t process_init()
{
    ESP_LOGI(TAG, "Initialised FIR filter with length %d", 
                             sizeof(fir_coeff) / sizeof(fir_coeff[0]));

    ESP_ERROR_CHECK(filter_init());
    ESP_ERROR_CHECK(fmv_init());
    ESP_ERROR_CHECK(gate_init(sizeof(fbuf)/sizeof(fbuf[0]), 0.1, 0.005, 10,
                50, 50, SAMPLE_RATE));

    chorus = new shrapnel::effect::Chorus();
    assert(chorus);
    chorus->set_sample_rate(SAMPLE_RATE);

    return dsps_fir_init_f32(&fir, fir_coeff, fir_delay_line,
                             sizeof(fir_coeff) / sizeof(fir_coeff[0]));

}
