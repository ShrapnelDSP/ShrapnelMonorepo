#pragma once

#include <array>
#include "dsps_biquad.h"
#include "iir.h"

namespace shrapnel {
namespace dsp {

template <size_t coefficient_count>
class IirFilterEsp final : public IirFilterBase {
    // TODO Use existing C implementation when order is greater than 2
    static_assert(coefficient_count == 6, "Only biquads are supported for now");

    public:
    IirFilterEsp(void)
    {
        coefficients.fill(0);
        reset();
    }

    void process(const float *in, float *out, size_t buf_size) override
    {
        dsps_biquad_f32_ae32(in, out, buf_size, coefficients.data(), delay.data());
    }

    void reset(void) override
    {
        delay.fill(0);
    }

    void set_coefficients(std::array<float, 6> new_coefficients) override
    {
        size_t out_index = 0;
        for(int i = 0; i < 6; i++)
        {
            if(i == 3)
            {
                continue;
            }

            coefficients[out_index] = new_coefficients[i] / new_coefficients[3];
            out_index++;
        }
    }

    private:
    std::array<float, coefficient_count> coefficients;

    std::array<float, (coefficient_count - 1) / 2 + 1> delay;
};

}
}
