#pragma once

#include <array>

namespace shrapnel {
namespace dsp {

class IirFilterBase {
    public:
    virtual void process(const float *in, float *out, size_t buf_size) = 0;

    virtual void reset(void) = 0;

    virtual void set_coefficients(std::array<float, 6> new_coefficients) = 0;
};

}
}
