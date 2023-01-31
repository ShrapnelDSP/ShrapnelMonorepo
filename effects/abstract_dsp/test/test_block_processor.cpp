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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "block_processor.h"

#define PROCESSOR_BLOCK_SIZE 2

class Negative20dBGainProcessor
{
public:
    void prepare(juce::dsp::ProcessSpec spec)
    {
        is_ready = true;
        EXPECT_THAT(spec.maximumBlockSize, PROCESSOR_BLOCK_SIZE);
    }

    void reset() { is_ready = false; }

    void process(juce::dsp::ProcessContextReplacing<float> context)
    {
        EXPECT_TRUE(is_ready);
        auto block = context.getOutputBlock();
        EXPECT_THAT(block.getNumSamples(), PROCESSOR_BLOCK_SIZE);
        EXPECT_THAT(block.getNumChannels(), 1);
        auto samples = block.getChannelPointer(0);
        std::for_each(samples,
                      samples + PROCESSOR_BLOCK_SIZE,
                      [](auto &value) { value *= 0.1; });
    }

private:
    bool is_ready = false;
};

class BlockProcessor : public ::testing::Test
{
protected:
    void SetUp() override { uut.prepare(juce::dsp::ProcessSpec{48e3, 3, 1}); }

    void TearDown() override { uut.reset(); }

    shrapnel::dsp::BlockProcessor<PROCESSOR_BLOCK_SIZE,
                                  Negative20dBGainProcessor>
        uut{Negative20dBGainProcessor()};
};

TEST_F(BlockProcessor, BlockSizedInputs)
{
    std::array<float, PROCESSOR_BLOCK_SIZE> samples{};
    float *samples_ptr = samples.data();
    float *const *channels = &samples_ptr;

    auto block =
        juce::dsp::AudioBlock<float>(channels, 1, PROCESSOR_BLOCK_SIZE);

    samples = {1.f, 2.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.f, 0.f));

    samples = {3.f, 4.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.1f, 0.2f));

    samples = {0.f, 0.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.3f, 0.4f));
}

TEST_F(BlockProcessor, SmallInputs)
{
    std::array<float, 1> samples{};
    float *samples_ptr = samples.data();
    float *const *channels = &samples_ptr;

    auto block = juce::dsp::AudioBlock<float>(channels, 1, 1);

    samples = {1.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.f));

    samples = {2.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.f));

    samples = {3.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.1f));

    samples = {4.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.2f));

    samples = {0.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.3f));

    samples = {0.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.4f));
}

TEST_F(BlockProcessor, LargeInputs)
{
    std::array<float, 3> samples{};
    float *samples_ptr = samples.data();
    float *const *channels = &samples_ptr;

    auto block = juce::dsp::AudioBlock<float>(channels, 1, 3);

    samples = {1.f, 2.f, 3.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.f, 0.f, 0.1f));

    samples = {4.f, 0.f, 0.f};
    uut.process(block);
    EXPECT_THAT(samples, testing::ElementsAre(0.2f, 0.3f, 0.4f));
}
