#include "delayline.h"

namespace shrapnel {
namespace dsp {

DelayLine::DelayLine(size_t max_samples) : length(max_samples) { }

void DelayLine::push_sample(float sample)
{
    this->sample = sample;
}

float DelayLine::pop_sample(void)
{
    return sample;
}

}
}
