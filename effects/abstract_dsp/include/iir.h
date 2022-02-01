#pragma once

#include <array>
#include <memory>

namespace shrapnel {
namespace dsp {

template <size_t coefficient_count>
class IirFilter {
    static_assert(coefficient_count == 5, "Only biquads are supported for now");

    public:
    IirFilter();

    void process(const float *in, float *out, size_t buf_size);
    void reset(const float *in, float *out, size_t buf_size);

    std::shared_ptr<std::array<float, coefficient_count>> coefficients;
};

}
}
