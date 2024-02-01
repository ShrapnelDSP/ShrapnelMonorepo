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

#include "audio_processor.h"
#include "esp_log.h"
#include "speaker_coeffs.h"
#include <noise_gate.h>

#define TAG "audio_processor"

namespace {

float decibel_to_ratio(float db) { return powf(10.f, db / 20.f); }

} // namespace

namespace shrapnel {

AudioProcessor::AudioProcessor(AudioProcessorParameters a_parameters)
    : parameters{a_parameters},
      speaker{{std::make_unique<shrapnel::dsp::FastConvolution<1024, 512>>(
          fir_coeff)}}
{
    esp_err_t rc = gate_init();
    assert(rc == ESP_OK);

    ESP_LOGI(TAG, "Initialised FIR filter with length %zd", fir_coeff.size());
}

void AudioProcessor::prepare(float sample_rate, size_t buffer_size)
{
    gate_set_sample_rate(sample_rate);
    gate_set_buffer_size(buffer_size);

    chorus.prepare(sample_rate, buffer_size);
    wah.prepare(sample_rate, buffer_size);
    valvestate.prepare(sample_rate, buffer_size);
}

void AudioProcessor::process(std::span<float, 512> fbuf)
{
    gate_set_threshold(*parameters.gate.threshold, *parameters.gate.hysteresis);
    gate_set_attack(*parameters.gate.attack);
    gate_set_hold(*parameters.gate.hold);
    gate_set_release(*parameters.gate.release);

    profiling_mark_stage("start");

    gate_analyse(fbuf.data(), fbuf.size());
    profiling_mark_stage("gate_analyze");

    valvestate.set_gain(*parameters.amplifier.gain,
                        *parameters.amplifier.channel);
    valvestate.set_fmv(*parameters.amplifier.bass,
                       *parameters.amplifier.middle,
                       *parameters.amplifier.treble);
    valvestate.set_contour(*parameters.amplifier.contour);
    valvestate.set_volume(decibel_to_ratio(*parameters.amplifier.volume));
    profiling_mark_stage("valvestate parameters");

    valvestate.process(fbuf);
    profiling_mark_stage("valvestate process");

    if(*parameters.gate.bypass < 0.5f)
    {
        gate_process(fbuf.data(), fbuf.size());
    }
    profiling_mark_stage("gate done");

    /* speaker IR */
    speaker.process(fbuf);
    profiling_mark_stage("speaker done");

    chorus.set_modulation_rate_hz(*parameters.chorus.rate);
    chorus.set_modulation_depth(*parameters.chorus.depth);
    chorus.set_modulation_mix(*parameters.chorus.mix);
    profiling_mark_stage("chorus parameters");

    if(*parameters.chorus.bypass < 0.5f)
    {
        chorus.process(fbuf);
    }
    profiling_mark_stage("chorus done");

    wah.set_expression_position(*parameters.wah.position);
    wah.set_vocal(*parameters.wah.vocal);

    if(*parameters.wah.bypass < 0.5f)
    {
        wah.process(fbuf);
    }
}

void AudioProcessor::reset()
{
    valvestate.reset();
    chorus.reset();
    wah.reset();
    speaker.reset();
}

} // namespace shrapnel