#include "delayline.h"
#include <cmath>

namespace shrapnel {
namespace dsp {

DelayLine::DelayLine(size_t max_samples) :
    samples(max_samples + 1),
    writeIndex(0) { }

void DelayLine::push_sample(float sample)
{
    samples[writeIndex] = sample;

    writeIndex++;
    writeIndex %= samples.size();
}

float DelayLine::pop_sample(void)
{
    auto length = samples.size();

    auto integral_delay = static_cast<size_t>(std::floor(delay));
    float fractional_delay = delay - integral_delay;

    auto last_written_index = (writeIndex - 1 + length) % length;
    auto before_last_written_index = (last_written_index - 1 + length) % length;

    auto sample = samples[(last_written_index - integral_delay + length) % length];
    auto before_sample = samples[(before_last_written_index - integral_delay + length) % length];

    return (1 - fractional_delay) * sample + fractional_delay * before_sample;
}

void DelayLine::set_delay(float new_delay)
{
    delay = std::min((float)samples.size() - 1, std::max(0.f, new_delay));
}

}
}
