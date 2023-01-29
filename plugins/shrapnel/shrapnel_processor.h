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