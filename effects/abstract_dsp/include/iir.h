#pragma once

#include <array>
#include <cstddef>

namespace shrapnel {
namespace dsp {

class IirFilterBase {
    public:
    virtual void process(const float *in, float *out, std::size_t buf_size) = 0;

    virtual void reset(void) = 0;

    virtual void set_coefficients(std::array<float, 6> new_coefficients) = 0;
    virtual void set_coefficients(std::array<float, 8> new_coefficients) = 0;
};

}
}
