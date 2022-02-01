#pragma once

#include <array>
#include <memory>

#include "dsps_biquad.h"

namespace shrapnel {
namespace dsp {

// TODO maybe the template parameter should be the filter order?
template <size_t coefficient_count>
class IirFilter {
    // TODO Use existing C implementation when order is grater than 2
    static_assert(coefficient_count == 5, "Only biquads are supported for now");

    public:
    IirFilter(void) : coefficients(nullptr)
    {
        reset();
    }

    void process(const float *in, float *out, size_t buf_size)
    {
        dsps_biquad_f32_ae32(in, out, buf_size, coefficients->data(), delay.data());
    }

    void reset(void)
    {
        delay.fill(0);
    }

    std::shared_ptr<std::array<float, coefficient_count>> coefficients;

    private:
    std::array<float, (coefficient_count - 1) / 2 + 1> delay;
};

}
}
