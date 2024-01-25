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

#pragma once

#include "chorus.h"
#include "fast_convolution.h"
#include "fast_fir.h"
#include "valvestate.h"
#include "wah.h"

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace shrapnel {

struct AudioProcessorParameters
{
    struct
    {
        std::atomic<float> *gain;
        std::atomic<float> *channel;
        std::atomic<float> *bass;
        std::atomic<float> *middle;
        std::atomic<float> *treble;
        std::atomic<float> *contour;
        std::atomic<float> *volume;
    } amplifier;

    struct
    {
        std::atomic<float> *threshold;
        std::atomic<float> *hysteresis;
        std::atomic<float> *attack;
        std::atomic<float> *hold;
        std::atomic<float> *release;
        std::atomic<float> *bypass;
    } gate;

    struct
    {
        std::atomic<float> *rate;
        std::atomic<float> *depth;
        std::atomic<float> *mix;
        std::atomic<float> *bypass;
    } chorus;

    struct
    {
        std::atomic<float> *position;
        std::atomic<float> *vocal;
        std::atomic<float> *bypass;
    } wah;
};

class AudioProcessor
{
public:
    explicit AudioProcessor(AudioProcessorParameters a_parameters);

    void prepare(float sample_rate, size_t buffer_size);
    // Buffer size is hardcoded because the FFT based FIR filter is not
    // adjustable
    void process(std::span<float, 512> samples);
    void reset();

private:
    AudioProcessorParameters parameters;

    effect::valvestate::Valvestate valvestate;
    effect::Chorus chorus;
    effect::Wah wah;

    shrapnel::dsp::FastFir<512, 1024, shrapnel::dsp::FastConvolution<1024, 512>>
        speaker;
};

static_assert(dsp::Processor<AudioProcessor, 512>);

}; // namespace shrapnel