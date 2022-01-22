#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "delayline.h"

#define DELAY_LENGTH 5

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
    constexpr int delay = 2;

    static_assert(delay < DELAY_LENGTH);
    static_assert(delay > 0);

    uut.set_delay(delay);

    for(int i = 0; i < delay; i++)
    {
        uut.push_sample(i);
        EXPECT_FLOAT_EQ(0, uut.pop_sample());
    }
}

TEST_F(DelayLine, IntegralDelay)
{
    constexpr int delay = 2;

    static_assert(delay < DELAY_LENGTH);
    static_assert(delay > 0);

    uut.set_delay(delay);

    for(int i = 0; i < DELAY_LENGTH; i++)
    {
        uut.push_sample(i);

        if(i < delay)
        {
            // these are already checked in the test above
            uut.pop_sample();
            continue;
        }

        EXPECT_FLOAT_EQ(i - delay, uut.pop_sample());
    }
}

TEST_F(DelayLine, FractionalDelay)
{
    constexpr float delay = 2.5;

    static_assert(delay < DELAY_LENGTH);
    static_assert(delay > 0);

    uut.set_delay(delay);

    for(int i = 0; i < DELAY_LENGTH; i++)
    {
        uut.push_sample(i);

        EXPECT_FLOAT_EQ(std::max(0.f, i - delay), uut.pop_sample());
    }
}

TEST_F(DelayLine, WrapAround)
{
    constexpr float delay = 2.5;

    static_assert(delay < DELAY_LENGTH);
    static_assert(delay > 0);

    uut.set_delay(delay);

    for(int i = 0; i < DELAY_LENGTH * 2; i++)
    {
        uut.push_sample(i);

        EXPECT_FLOAT_EQ(std::max(0.f, i - delay), uut.pop_sample());
    }
}

TEST_F(DelayLine, NegativeDelayIsClippedToZero)
{
    uut.set_delay(-1);

    for(int i = 0; i < DELAY_LENGTH; i++)
    {
        uut.push_sample(i);
        EXPECT_FLOAT_EQ(i, uut.pop_sample());
    }
}
