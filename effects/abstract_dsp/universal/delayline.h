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
    std::vector<float> samples;

    size_t writeIndex;

    float delay;
};

}
}
