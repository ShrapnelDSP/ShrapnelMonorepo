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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audio_param.h"
#include <memory>

class AudioParams : public ::testing::Test
{
    protected:

    using AudioParameters = shrapnel::AudioParameters;

    AudioParams()
    {
        uut.create_and_add_parameter("test", 0, 10, 5);
    }

    AudioParameters uut;
};

TEST_F(AudioParams, UpdateReturnsZeroOnSuccess)
{
    ASSERT_EQ(0, uut.update("test", 0.5));
}

TEST_F(AudioParams, UpdateReturnsNonZeroOnFailure)
{
    ASSERT_NE(0, uut.update("invalid", 0.5));
}

TEST_F(AudioParams, DefaultValueIsCorrect)
{
    ASSERT_EQ(5, *uut.get_raw_parameter("test"));
}

TEST_F(AudioParams, UpdateToMinimum)
{
    uut.update("test", 0);

    ASSERT_EQ(0, *uut.get_raw_parameter("test"));
}

TEST_F(AudioParams, UpdateToMaximum)
{
    uut.update("test", 1);

    ASSERT_EQ(10, *uut.get_raw_parameter("test"));
}

TEST_F(AudioParams, UpdateToHalfWithNonTrivialRange)
{
    uut.create_and_add_parameter("non-trivial", -1, 1, -1);
    uut.update("non-trivial", 0.5);

    ASSERT_EQ(0, *uut.get_raw_parameter("non-trivial"));
}

TEST_F(AudioParams, UpdateTooHighIsNoop)
{
    uut.update("test", 1.1);

    ASSERT_EQ(5, *uut.get_raw_parameter("test"));
}

TEST_F(AudioParams, UpdateTooLowIsNoop)
{
    uut.update("test", -0.1);

    ASSERT_EQ(5, *uut.get_raw_parameter("test"));
}

TEST_F(AudioParams, UpdateLimitEdgeCases)
{
    uut.update("test", 0);
    ASSERT_EQ(0, *uut.get_raw_parameter("test"));

    uut.update("test", 1);
    ASSERT_EQ(10, *uut.get_raw_parameter("test"));
}
