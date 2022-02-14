#pragma once

#include <array>
#include "dsps_biquad.h"
#include "iir.h"
#include "iir_universal.h"

namespace shrapnel {
namespace dsp {

class IirFilter final : public IirFilterBase {
    public:
    IirFilter(void) : order(2)
    {
        coefficients.fill(0);
        reset();
    }

    void process(const float *in, float *out, size_t buf_size) override
    {
        if(order == 2)
        {
            dsps_biquad_f32_ae32(in, out, buf_size, coefficients.data(), delay.data());
        }
        else
        {
            iir_process(in, out, buf_size, coefficients.data(), delay.data(), order * 2 + 2);
        }
    }

    void reset(void) override
    {
        delay.fill(0);
    }

    void set_coefficients(std::array<float, 6> new_coefficients) override
    {
        order = 2;

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

    void set_coefficients(std::array<float, 8> new_coefficients) override
    {
        order = 3;
        for(int i = 0; i < new_coefficients.size(); i++)
        {
            coefficients[i] = new_coefficients[i];
        }
    }

    void set_coefficients(std::array<float, 10> new_coefficients) override
    {
        order = 4;
        coefficients = new_coefficients;
    }

    private:
    std::array<float, 10> coefficients;
    std::array<float, 4> delay;
    int order;
};

}
}
