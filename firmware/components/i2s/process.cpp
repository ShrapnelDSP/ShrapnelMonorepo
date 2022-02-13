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
#include "float_convert.h"
#include "dsps_fir.h"
#include "dsps_mulc.h"
#include "dsps_mul.h"
#include "math.h"
#include "iir.h"
#include "audio_events.h"
#include "noise_gate.h"
#include "profiling.h"
#include "chorus.h"
#include "valvestate.h"

#include "esp_log.h"
#define TAG "i2s_process"

#include "driver/gpio.h"

#include <assert.h>
#include <stdlib.h>
#include <atomic>

namespace {

std::atomic<float> *amp_gain;
std::atomic<float> *amp_channel;
std::atomic<float> *bass;
std::atomic<float> *middle;
std::atomic<float> *treble;
std::atomic<float> *contour;
std::atomic<float> *volume;

std::atomic<float> *gate_threshold;

shrapnel::effect::valvestate::Valvestate *valvestate;
shrapnel::effect::Chorus *chorus;

}

extern gpio_num_t g_profiling_gpio;
static float fbuf[DMA_BUF_SIZE];

#include "speaker_coeffs.h"
static float fir_delay_line[sizeof(fir_coeff)/sizeof(fir_coeff[0])];
static fir_f32_t fir;

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

    gate_set_threshold(*gate_threshold, 1);

    profiling_mark_stage(0);

    gate_analyse(fbuf, buf_len/2);
    profiling_mark_stage(1);
    profiling_mark_stage(2);
    profiling_mark_stage(3);

    profiling_mark_stage(4);

    profiling_mark_stage(5);

    profiling_mark_stage(6);

    profiling_mark_stage(7);
    valvestate->set_gain(*amp_gain, *amp_channel);
    valvestate->set_fmv(*bass, *middle, *treble);
    valvestate->set_contour(*contour);
    valvestate->set_volume(decibel_to_ratio(*volume));
    valvestate->process(fbuf, buf_len/2);

    profiling_mark_stage(8);

    profiling_mark_stage(9);
    gate_process(fbuf, buf_len/2);
    profiling_mark_stage(10);
    profiling_mark_stage(11);

    /* speaker IR */
    dsps_fir_f32_ae32(&fir, fbuf, fbuf, buf_len/2);
    profiling_mark_stage(12);

    chorus->set_modulation_rate_hz(1.f);
    chorus->set_modulation_depth(.5f);
    chorus->set_modulation_mix(.8f);
    chorus->process(fbuf, buf_len/2);
    profiling_mark_stage(13);

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

    valvestate = new shrapnel::effect::valvestate::Valvestate();
    assert(valvestate);
    valvestate->prepare(SAMPLE_RATE);

    amp_gain = audio_params->get_raw_parameter("ampGain");
    assert(amp_gain);
    amp_channel = audio_params->get_raw_parameter("ampChannel");
    assert(amp_channel);
    bass = audio_params->get_raw_parameter("bass");
    assert(bass);
    middle = audio_params->get_raw_parameter("middle");
    assert(middle);
    treble = audio_params->get_raw_parameter("treble");
    assert(treble);
    contour = audio_params->get_raw_parameter("contour");
    assert(contour);
    volume = audio_params->get_raw_parameter("volume");
    assert(volume);

    gate_threshold = audio_params->get_raw_parameter("gateThreshold");
    assert(gate_threshold);

    return dsps_fir_init_f32(&fir, fir_coeff, fir_delay_line,
                             sizeof(fir_coeff) / sizeof(fir_coeff[0]));

}
