#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "delayline.h"

#define DELAY_LENGTH 100

class DelayLine : public ::testing::Test
{
    protected:
    DelayLine() : uut(DELAY_LENGTH) {}

    shrapnel::dsp::DelayLine uut;
};

TEST_F(DelayLine, UnityByDefault)
{
    for(int i = 0; i < DELAY_LENGTH; i++)
    {
        uut.push_sample(i);
        EXPECT_FLOAT_EQ(i, uut.pop_sample());
    }
}
