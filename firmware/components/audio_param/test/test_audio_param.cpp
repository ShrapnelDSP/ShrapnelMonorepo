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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "audio_param.h"
#include <memory>

using namespace shrapnel;

class AudioParams : public ::testing::Test
{
protected:
    using AudioParameters = parameters::AudioParameters<3, 1>;

    AudioParams()
    {
        int rc = uut.create_and_add_parameter("test", 0, 10, 5);
        assert(rc == 0);
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
    (void)uut.create_and_add_parameter("non-trivial", -1, 1, -1);
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

TEST_F(AudioParams, Iterate)
{
    (void)uut.create_and_add_parameter("test1", 0, 10, 1);
    (void)uut.create_and_add_parameter("test2", 0, 10, 2);

    std::map<parameters::id_t, float> expected{
        {"test", 5}, // defined in test fixture
        {"test1", 1},
        {"test2", 2},
    };

    std::map<parameters::id_t, float> actual{};

    for(const auto &[key, value] : uut)
    {
        actual[key] = *value->get_raw_parameter();
    }

    EXPECT_EQ(expected.size(), actual.size());

    for(const auto &[key, value] : expected)
    {
        EXPECT_EQ(value, actual[key]);
    }
}

TEST_F(AudioParams, GetNormalised) { EXPECT_EQ(0.5, uut.get("test")); }
