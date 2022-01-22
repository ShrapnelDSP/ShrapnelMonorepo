#include "delayline.h"
#include <cmath>

namespace shrapnel {
namespace dsp {

DelayLine::DelayLine(size_t max_samples) :
    samples(max_samples),
    writeIndex(0) { }

void DelayLine::push_sample(float sample)
{
    this->samples[writeIndex] = sample;
    writeIndex++;
}

float DelayLine::pop_sample(void)
{
    auto length = samples.size();

    auto integral_delay = static_cast<size_t>(std::floor(delay));
    auto last_written_index = (writeIndex - 1 + length) % length;

    return samples[(last_written_index - integral_delay + length) % length];
}


void DelayLine::set_delay(float new_delay)
{
    delay = new_delay;
}

}
}
