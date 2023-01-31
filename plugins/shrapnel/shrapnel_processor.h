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

#include "audio_processor.h"
#include "block_processor.h"
#include <juce_dsp/juce_dsp.h>

class ShrapnelAudioProcessor : juce::dsp::ProcessorBase
{
public:
    static constexpr size_t block_size = 512;

    explicit ShrapnelAudioProcessor(
        shrapnel::AudioProcessorParameters parameters)
        : processor{parameters}
    {
    }

    void prepare(const juce::dsp::ProcessSpec &spec) override
    {
        jassert(spec.numChannels == 1);
        jassert(spec.maximumBlockSize == block_size);

        processor.prepare(spec.sampleRate, block_size);
    }

    void reset() override
    {
        processor.reset();
    }

    void process(const juce::dsp::ProcessContextReplacing<float> &context) override
    {
        jassert(context.getOutputBlock().getNumSamples() == block_size);

        auto samples = context.getOutputBlock().getChannelPointer(0);
        processor.process(std::span<float, block_size>(samples, block_size));

    }

private:
    shrapnel::AudioProcessor processor;
};