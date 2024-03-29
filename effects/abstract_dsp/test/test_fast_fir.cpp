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
 *
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "fast_fir.h"

void profiling_mark_stage(unsigned int stage) { (void)stage; }

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::FloatEq;

class FastFir : public ::testing::Test
{
};

template <size_t N>
class MockFastConvolution final
{
public:
    MOCK_METHOD(void,
                process,
                ((const std::array<float, N> &b), (std::array<float, N> & out)),
                ());
};

TEST_F(FastFir, SignalIsBufferedCorrectly)
{
    auto convolution = std::make_unique<MockFastConvolution<8>>();

    auto input = std::array{
        std::array{10.f, 20.f},
        std::array{30.f, 40.f},
        std::array{50.f, 60.f},
        std::array{70.f, 80.f},
        std::array{90.f, 100.f},
    };

    ::testing::InSequence seq;

    EXPECT_CALL(
        *convolution,
        process(ElementsAre(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 10.f, 20.f), _))
        .Times(1);

    EXPECT_CALL(
        *convolution,
        process(ElementsAre(0.f, 0.f, 0.f, 0.f, 10.f, 20.f, 30.f, 40.f), _))
        .Times(1);

    EXPECT_CALL(
        *convolution,
        process(ElementsAre(0.f, 0.f, 10.f, 20.f, 30.f, 40.f, 50.f, 60.f), _))
        .Times(1);

    EXPECT_CALL(
        *convolution,
        process(ElementsAre(10.f, 20.f, 30.f, 40.f, 50.f, 60.f, 70.f, 80.f), _))
        .Times(1);

    EXPECT_CALL(
        *convolution,
        process(ElementsAre(30.f, 40.f, 50.f, 60.f, 70.f, 80.f, 90.f, 100.f),
                _))
        .Times(1);

    shrapnel::dsp::FastFir<2, 8, MockFastConvolution<8>> uut(
        std::move(convolution));

    for(auto &in : input)
    {
        uut.process(in);
    }
}

TEST_F(FastFir, OutputIsCopiedCorrectly)
{
    auto convolution = std::make_unique<MockFastConvolution<8>>();

    std::array<float, 2> signal{};

    EXPECT_CALL(*convolution, process(_, _))
        .WillOnce(::testing::SetArgReferee<1>(
            std::array{8.f, 7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f}));

    shrapnel::dsp::FastFir<2, 8, MockFastConvolution<8>> uut(
        std::move(convolution));

    uut.process(signal);
    EXPECT_THAT(signal, ElementsAre(2.f, 1.f));
}
