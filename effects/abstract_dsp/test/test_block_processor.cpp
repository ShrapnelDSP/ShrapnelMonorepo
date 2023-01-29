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

class BlockProcessor : public ::testing::Test
{
protected:
    void SetUp() override { uut.prepare(juce::dsp::ProcessSpec{48e3, 8, 1}); }

    void TearDown() override { uut.reset(); }

    shrapnel::dsp::BlockProcessor<2> uut;
};

TEST_F(BlockProcessor, BlockSizedInputs)
{
    std::array<float, 2> samples{};
    float *samples_ptr = samples.data();
    float *const *channels = &samples_ptr;

    auto block = juce::dsp::AudioBlock<float>(channels, 1, 2);

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
