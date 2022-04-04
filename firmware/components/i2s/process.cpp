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
#include "dsps_mulc.h"
#include "dsps_mul.h"
#include "math.h"
#include "iir.h"
#include "audio_events.h"
#include "noise_gate.h"
#include "profiling.h"
#include "chorus.h"
#include "valvestate.h"
#include "fast_convolution.h"
#include "fast_fir.h"

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
std::atomic<float> *gate_hysteresis;
std::atomic<float> *gate_attack;
std::atomic<float> *gate_hold;
std::atomic<float> *gate_release;
std::atomic<float> *gate_bypass;

std::atomic<float> *chorus_rate;
std::atomic<float> *chorus_depth;
std::atomic<float> *chorus_mix;
std::atomic<float> *chorus_bypass;

shrapnel::effect::valvestate::Valvestate *valvestate;
shrapnel::effect::Chorus *chorus;

shrapnel::dsp::FastConvolution<256> *speaker_convolution;
shrapnel::dsp::FastFir<
    DMA_BUF_SIZE/2,
    256,
    128,
    shrapnel::dsp::FastConvolution<256>> *speaker;

}

extern gpio_num_t g_profiling_gpio;
static float fbuf[DMA_BUF_SIZE];

#include "speaker_coeffs.h"

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

    gate_set_threshold(*gate_threshold, *gate_hysteresis);
    gate_set_attack(*gate_attack);
    gate_set_hold(*gate_hold);
    gate_set_release(*gate_release);

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

    if(*gate_bypass < 0.5f)
    {
        gate_process(fbuf, buf_len/2);
    }

    profiling_mark_stage(10);
    profiling_mark_stage(11);

    /* speaker IR */
    speaker->process(fbuf);
    profiling_mark_stage(12);

    chorus->set_modulation_rate_hz(*chorus_rate);
    chorus->set_modulation_depth(*chorus_depth);
    chorus->set_modulation_mix(*chorus_mix);

    profiling_mark_stage(13);

    if(*chorus_bypass < 0.5f)
    {
        chorus->process(fbuf, buf_len/2);
    }

    profiling_mark_stage(14);

    for(int i = 1; i < buf_len; i+=2)
    {
        if(fbuf[i/2] > 1.f || fbuf[i/2] < -1.f)
        {
            xEventGroupSetBits(g_audio_event_group, AUDIO_EVENT_OUTPUT_CLIPPED);
        }

        if(fbuf[i] != fbuf[i])
        {
            ESP_LOGE(TAG, "Not a number");
            assert(0);
        }

        buf[i] = float_to_int32(fbuf[i/2]);
    }

    profiling_mark_stage(15);
    profiling_stop();
    gpio_set_level(g_profiling_gpio, 0);
}

esp_err_t process_init(shrapnel::AudioParameters *audio_params)
{
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

    gate_threshold = audio_params->get_raw_parameter("noiseGateThreshold");
    assert(gate_threshold);
    gate_hysteresis = audio_params->get_raw_parameter("noiseGateHysteresis");
    assert(gate_hysteresis);
    gate_attack = audio_params->get_raw_parameter("noiseGateAttack");
    assert(gate_attack);
    gate_hold = audio_params->get_raw_parameter("noiseGateHold");
    assert(gate_hold);
    gate_release = audio_params->get_raw_parameter("noiseGateRelease");
    assert(gate_release);
    gate_bypass = audio_params->get_raw_parameter("noiseGateBypass");
    assert(gate_bypass);

    chorus_rate = audio_params->get_raw_parameter("chorusRate");
    assert(chorus_rate);
    chorus_depth = audio_params->get_raw_parameter("chorusDepth");
    assert(chorus_depth);
    chorus_mix = audio_params->get_raw_parameter("chorusMix");
    assert(chorus_mix);
    chorus_bypass = audio_params->get_raw_parameter("chorusBypass");
    assert(chorus_bypass);

    ESP_LOGI(TAG, "Initialised FIR filter with length %d", fir_coeff.size());

    speaker_convolution = new shrapnel::dsp::FastConvolution<256>();
    assert(speaker_convolution);

    speaker = new shrapnel::dsp::FastFir<
        DMA_BUF_SIZE/2,
        256,
        128,
        shrapnel::dsp::FastConvolution<256>>(fir_coeff, *speaker_convolution);
    assert(speaker);

    return ESP_OK;
}
