#pragma once

#include "FixedBlockProcessor.h"
#include "audio_processor.h"
#include <juce_dsp/juce_dsp.h>

class ShrapnelAudioProcessor final : public FixedBlockProcessor
{
public:
    explicit ShrapnelAudioProcessor(
        shrapnel::AudioProcessorParameters parameters)
        : FixedBlockProcessor(512), processor{parameters}
    {
    }

    void prepare(const juce::dsp::ProcessSpec &spec) override
    {
        FixedBlockProcessor::prepare(spec);
        processor.prepare(spec.sampleRate,
                          static_cast<size_t>(getMaximumBlockSize()));
    }

    void reset()
    {
        resetFrame();
        processor.reset();
    }

    void process(const juce::dsp::ProcessContextReplacing<float> &context)
    {
        // TODO This is certainly not correct, we need to pull data out of the buffer when it is available
        jassert(!context.usesSeparateInputAndOutputBlocks());

        const auto &block = context.getOutputBlock();

        for(size_t i = 0; i < block.getNumChannels(); i++)
        {
            // This might call performProcessing multiple times, how do we write to the output?
            appendData(i, block.getNumSamples(), block.getChannelPointer(i));
        }
    }

    void performProcessing(int channel) override
    {
        jassert(buffer.getNumSamples() == 512);

        // TODO handle multichannel correctly
        if(channel == 0)
        {
            auto samples = buffer.getWritePointer(channel);
            processor.process(std::span<float, 512>(
                samples, static_cast<size_t>(buffer.getNumSamples())));
        }
        else
        {
            buffer.clear(channel, 0, buffer.getNumSamples());
        }
    }

private:
    shrapnel::AudioProcessor processor;
};