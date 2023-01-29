/*
 * This file is copied from DSP-Testbench, available under the following license:
 *
 * MIT License
 *
 * Copyright (c) 2018 Andrew J
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>

/**
 * This abstract class is used to provide the capability to stream data to
 * elsewhere in a real time audio process at a fixed block size which is
 * independent of the block size of the source stream. Derivations need to
 * implement the performProcessing() method. It is assumed that processing
 * will take place on the same thread as AudioProcessor::processBlock(),
 * there is no consideration given to thread safety.
 *
 * Note that this class only operates on float based audio data.
 */
class FixedBlockProcessor
{
public:
    /** We set a maximum block size at construction but allow later modification
     * to a smaller size if necessary.
     */
    explicit FixedBlockProcessor(const int maximumBlockSize)
        : maxBlockSize(maximumBlockSize), currentBlockSize(maximumBlockSize)
    {
    }

    virtual ~FixedBlockProcessor() = default;

    /** Prepare the FixedBlockProcessor. Note that this only sets the number of
     * channels. The blocksize of spec is typically different to that used here,
     * so call \ref modifyCurrentBlockSize instead. */
    virtual void prepare(const juce::dsp::ProcessSpec &spec)
    {
        jassert(spec.numChannels > 0);
        numChannels = static_cast<int>(spec.numChannels);
        resizeBuffer();
    }

    /** Gets the number of channels. */
    [[nodiscard]] int getNumChannels() const { return numChannels; }

    /** Sets the current block size used. Current size is initialised to maximum
     * size. If you set a smaller size, then \ref performProcessing will be
     * called when the smaller, current block size is reached. If the size is
     * reduced while there is already data in the buffer, then that data will be
     * cleared and will not be processed.
     *
     * \param size must be less than or equal to the maximum, larger values will
     *             be truncated to max.
     */
    void modifyCurrentBlockSize(const int size)
    {
        jassert(size > 0);
        jassert(size <= maxBlockSize);
        currentBlockSize = juce::jmin(size, maxBlockSize);
        resetFrame();
    }

    /** Gets the maximum block size. */
    [[nodiscard]] int getMaximumBlockSize() const { return maxBlockSize; }

    /** Gets the current block size. */
    [[nodiscard]] int getCurrentBlockSize() const { return currentBlockSize; }

    /** Resets the current write to the start of the frame (for each channel). */
    void resetFrame()
    {
        for(auto ch = 0; ch < numChannels; ++ch)
            currentIndex[ch] = 0;
    }

    /** Appends data to the buffer.
     * \param channel Channel to which data is going to be appended
     * \param numSamples Number of samples to be appended
     * \param data Pointer to source data
     */
    void appendData(const int channel, const int numSamples, const float *data)
    {
        // If this assert fires then you probably haven't called prepare()
        jassert(numChannels > 0);
        jassert(currentBlockSize > 0);
        jassert(channel >= 0 && channel < numChannels);
        // If this assert fires then you probably haven't called prepare()
        jassert(numSamples > 0);

        if(data != nullptr)
        {
            if(currentIndex[channel] + numSamples < currentBlockSize)
            {
                // Data can't fill the channel buffer, so write what we have
                buffer.copyFrom(
                    channel, currentIndex[channel], data, numSamples);
                currentIndex[channel] += numSamples;
            }
            else
            {
                // We have more data than can fit in the channel buffer
                auto samplesRemaining = numSamples;
                auto dataOffset = 0;

                // Write remainder of channel buffer and process
                const auto rem = currentBlockSize - currentIndex[channel];
                buffer.copyFrom(channel, currentIndex[channel], data, rem);
                performProcessing(channel);
                currentIndex[channel] = 0;
                samplesRemaining -= rem;
                dataOffset += rem;

                // Loop full blocks (processing as we go)
                while(samplesRemaining >= currentBlockSize)
                {
                    buffer.copyFrom(channel,
                                    currentIndex[channel],
                                    data + dataOffset,
                                    currentBlockSize);
                    performProcessing(channel);
                    samplesRemaining -= currentBlockSize;
                    dataOffset += currentBlockSize;
                }

                if(samplesRemaining > 0)
                {
                    // Write remainder of input
                    buffer.copyFrom(channel,
                                    currentIndex[channel],
                                    data + dataOffset,
                                    samplesRemaining);
                    currentIndex[channel] = samplesRemaining;
                }
            }
        }
    }

protected:
    /** Called whenever the fixed size buffer has been filled.
     *
     * \param channel Channel for which processing is to be performed
     */
    virtual void performProcessing(int channel) = 0;

    /** Internal buffer */
    juce::AudioSampleBuffer buffer;

    /** Used to hold current position in buffer (per channel) */
    juce::HeapBlock<int> currentIndex{};

private:
    /* This always allocates sufficient memory to hold the maximum block size. */
    void resizeBuffer()
    {
        buffer.setSize(numChannels, maxBlockSize, false, true, true);
        currentIndex.allocate(numChannels, true);
    }

    int numChannels = 0;
    int maxBlockSize = 0;
    int currentBlockSize = 0;

public:
    // Declare non-copyable, non-movable
    FixedBlockProcessor(const FixedBlockProcessor &) = delete;
    FixedBlockProcessor &operator=(const FixedBlockProcessor &) = delete;
    FixedBlockProcessor(FixedBlockProcessor &&other) = delete;
    FixedBlockProcessor &operator=(FixedBlockProcessor &&other) = delete;
};