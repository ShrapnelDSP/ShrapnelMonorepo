#pragma once
#include <cstddef>

namespace shrapnel {
namespace dsp {

class DelayLine {
    public:
    DelayLine(size_t max_samples);

    void push_sample(float sample);
    float pop_sample(void);

    void set_delay(float sample);

    private:
    size_t length;
    float sample;
};

}
}
