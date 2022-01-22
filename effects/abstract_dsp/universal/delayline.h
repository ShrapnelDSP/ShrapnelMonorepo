#pragma once
#include <cstddef>
#include <vector>

namespace shrapnel {
namespace dsp {

class DelayLine {
    public:
    DelayLine(size_t max_samples);

    void push_sample(float sample);
    float pop_sample(void);

    void set_delay(float new_delay);

    private:
    size_t length;
    std::vector<float> samples;

    size_t writeIndex;
    size_t readIndex;

    float delay;
};

}
}
