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
    auto uut = AudioParameters(std::make_unique<shrapnel::AudioParameterFloat> (
                    "test",
                    0,
                    10,
                    5)
            );

    ASSERT_EQ(0, uut.update("test", 0.5));
}
