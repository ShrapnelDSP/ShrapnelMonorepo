/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

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
#include <atomic>

namespace {

std::atomic<float> *tight;
std::atomic<float> *pedal_gain;
std::atomic<float> *amp_gain;
std::atomic<float> *bass;
std::atomic<float> *middle;
std::atomic<float> *treble;
std::atomic<float> *volume;
std::atomic<float> *gate_threshold;

}


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

static float decibel_to_ratio(float db)
{
    return powf(10.f, db / 20.f);
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

    fmv_update_params(*bass, *middle, *treble);
    filter_set_tight(*tight > 0.5f);
    gate_set_threshold(*gate_threshold, 1);

    profiling_mark_stage(0);

    gate_analyse(fbuf, buf_len/2);
    profiling_mark_stage(1);
    dsps_mulc_f32_ae32(fbuf, fbuf, sizeof(fbuf)/sizeof(fbuf[0]), decibel_to_ratio(*pedal_gain), 1, 1);
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

    dsps_mulc_f32_ae32(fbuf, fbuf, buf_len/2, decibel_to_ratio(*amp_gain), 1, 1);
    profiling_mark_stage(7);

    for(int i = 1; i < buf_len; i+=2)
    {
        fbuf[i/2] = waveshape(fbuf[i/2]);
    }
    profiling_mark_stage(8);

    fmv_process(fbuf, buf_len/2);
    profiling_mark_stage(9);
    gate_process(fbuf, buf_len/2);
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
    profiling_mark_stage(13);

    dsps_mulc_f32_ae32(fbuf, fbuf, buf_len/2, decibel_to_ratio(*volume), 1, 1);
    profiling_mark_stage(14);

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

    profiling_mark_stage(15);
    profiling_stop();
    gpio_set_level(g_profiling_gpio, 0);
}

esp_err_t process_init(shrapnel::AudioParametersBase *audio_params)
{
    ESP_LOGI(TAG, "Initialised FIR filter with length %d", 
                             sizeof(fir_coeff) / sizeof(fir_coeff[0]));

    ESP_ERROR_CHECK(filter_init(SAMPLE_RATE));
    ESP_ERROR_CHECK(fmv_init(SAMPLE_RATE));

    ESP_ERROR_CHECK(gate_init());
    gate_set_sample_rate(SAMPLE_RATE);
    gate_set_threshold(-60, 1);
    gate_set_attack(10);
    gate_set_hold(50);
    gate_set_release(50);
    gate_set_buffer_size(DMA_BUF_SIZE);

    chorus = new shrapnel::effect::Chorus();
    assert(chorus);
    chorus->set_sample_rate(SAMPLE_RATE);

    tight = audio_params->get_raw_parameter("tight");
    assert(tight);
    pedal_gain = audio_params->get_raw_parameter("hmGain");
    assert(pedal_gain);
    amp_gain = audio_params->get_raw_parameter("ampGain");
    assert(amp_gain);
    bass = audio_params->get_raw_parameter("bass");
    assert(bass);
    middle = audio_params->get_raw_parameter("middle");
    assert(middle);
    treble = audio_params->get_raw_parameter("treble");
    assert(treble);
    volume = audio_params->get_raw_parameter("volume");
    assert(volume);
    gate_threshold = audio_params->get_raw_parameter("gateThreshold");
    assert(gate_threshold);

    return dsps_fir_init_f32(&fir, fir_coeff, fir_delay_line,
                             sizeof(fir_coeff) / sizeof(fir_coeff[0]));

}
