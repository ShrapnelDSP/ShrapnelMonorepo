#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audio_param.h"
#include <memory>

class AudioParams : public ::testing::Test
{
    protected:

    using AudioParameters = shrapnel::AudioParameters;

    AudioParams() {}
};

TEST_F(AudioParams, UpdateReturnsZeroOnSuccess)
{
    auto uut = AudioParameters();
    uut.createAndAddParameter("test", 0, 10, 5);

    ASSERT_EQ(0, uut.update("test", 0.5));
}

TEST_F(AudioParams, UpdateReturnsNonZeroOnFailure)
{
    auto uut = AudioParameters();
    uut.createAndAddParameter("test", 0, 10, 5);

    ASSERT_NE(0, uut.update("invalid", 0.5));
}

TEST_F(AudioParams, DefaultValueIsCorrect)
{
    auto uut = AudioParameters();
    uut.createAndAddParameter("test", 0, 10, 5);

    ASSERT_EQ(5, *uut.get_raw_parameter("test"));
}

TEST_F(AudioParams, UpdateToMinimum)
{
    auto uut = AudioParameters();
    uut.createAndAddParameter("test", 0, 10, 5);
    uut.update("test", 0);

    ASSERT_EQ(0, *uut.get_raw_parameter("test"));
}

TEST_F(AudioParams, UpdateToMaximum)
{
    auto uut = AudioParameters();
    uut.createAndAddParameter("test", 0, 10, 5);
    uut.update("test", 1);

    ASSERT_EQ(10, *uut.get_raw_parameter("test"));
}

TEST_F(AudioParams, UpdateToHalfWithNonTrivialRange)
{
    auto uut = AudioParameters();
    uut.createAndAddParameter("test", -1, 1, -1);
    uut.update("test", 0.5);

    ASSERT_EQ(0, *uut.get_raw_parameter("test"));
}
