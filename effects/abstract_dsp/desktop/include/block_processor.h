#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>

namespace shrapnel::dsp {

template <size_t N, typename ChildProcessor>
class BlockProcessor : public juce::dsp::ProcessorBase
{
public:
    explicit BlockProcessor(ChildProcessor a_child) : child{std::move(a_child)} {}

    void prepare(const juce::dsp::ProcessSpec &spec) override
    {
        reset();

        auto childSpec = spec;
        childSpec.maximumBlockSize = N;
        child.prepare(childSpec);
    };

    void reset() override
    {
        buffers.clear();
        child.reset();
    };

    void
    process(const juce::dsp::ProcessContextReplacing<float> &context) override
    {
        auto io_block = context.getOutputBlock();
        auto samples_left = io_block.getNumSamples();
        size_t output_done_count = 0;

        while(samples_left > 0)
        {
            auto space_left_in_write_buffer = N - write_size;
            auto samples_processed_this_iteration =
                juce::jmin(space_left_in_write_buffer, samples_left);
            samples_left -= samples_processed_this_iteration;

            // We must never overflow the buffer
            jassert(write_size + samples_processed_this_iteration <= N);

            // Add input samples to the internal buffer
            auto write_buffer = buffers.getWritePointer(!read_buffer_channel);
            std::copy(io_block.getChannelPointer(0) + output_done_count,
                      io_block.getChannelPointer(0) + output_done_count +
                          samples_processed_this_iteration,
                      &write_buffer[write_size]);
            write_size += samples_processed_this_iteration;

            // Take outputs samples from the internal buffer
            auto read_buffer = buffers.getReadPointer(read_buffer_channel);
            std::copy(
                &read_buffer[read_size],
                &read_buffer[read_size + samples_processed_this_iteration],
                &io_block.getChannelPointer(0)[output_done_count]);
            output_done_count += samples_processed_this_iteration;
            read_size += samples_processed_this_iteration;

            jassert(read_size == write_size);

            // After pushing, the buffer might now be full. If it is, then we
            // let the block processor process it.
            if(write_size == N)
            {
                auto completeBlock =
                    block.getSingleChannelBlock(!read_buffer_channel);
                child.process(completeBlock);
                read_size = 0;
                write_size = 0;
                read_buffer_channel = !read_buffer_channel;
            }
        }
    };

private:
    ChildProcessor child;
    juce::AudioBuffer<float> buffers{2, N};
    juce::dsp::AudioBlock<float> block{buffers};
    size_t read_buffer_channel = 0;
    size_t write_size = 0;
    size_t read_size = 0;
};

} // namespace shrapnel::dsp