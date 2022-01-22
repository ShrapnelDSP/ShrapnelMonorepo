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

// NOTE: These tests should use less samples than the delay line length to
// avoid testing the overflow login. Only the later tests should fail if the
// overflow logic is broken.
TEST_F(DelayLine, UnityByDefault)
{
    for(int i = 0; i < DELAY_LENGTH; i++)
    {
        uut.push_sample(i);
        EXPECT_FLOAT_EQ(i, uut.pop_sample());
    }
}

TEST_F(DelayLine, StateInitialisedToZero)
{
    constexpr int delay = 10;

    static_assert(delay < DELAY_LENGTH);
    static_assert(delay > 0);

    uut.set_delay(delay);

    for(int i = 0; i < delay; i++)
    {
        uut.push_sample(i);
        EXPECT_FLOAT_EQ(0, uut.pop_sample());
    }
}
