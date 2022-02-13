#pragma once

#include <array>
#include "dsps_biquad.h"
#include "iir.h"

namespace {

// TODO this is copy pasted from the firmware IIR component
void iir_process(const float *input, float *output, int len, float *coeffs, float *w, int coeffs_len)
{
    float out_tmp;
    float *b = coeffs;
    float *a = &coeffs[coeffs_len/2];
    for(int i = 0; i < len; i++)
    {
        //calculate output
        out_tmp = (w[0] + b[0] * input[i])/a[0];

        for(int j = 0; j < coeffs_len/2 - 1; j++)
        {
            //update the state
            w[j] = 0;
            w[j] -= a[j+1] * out_tmp;
            w[j] += b[j+1] * input[i];
            /* add the next state variable down, except when at the last one */
            if(j < coeffs_len/2 - 2)
            {
                w[j] += w[j+1];
            }
        }
        output[i] = out_tmp;
    }
}

}

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

        size_t out_index = 0;
        for(int i = 0; i < 8; i++)
        {
            if(i == 3)
            {
                continue;
            }

            coefficients[out_index] = new_coefficients[i] / new_coefficients[3];
            out_index++;
        }
    }

    void set_coefficients(std::array<float, 10> new_coefficients) override
    {
        order = 4;

        size_t out_index = 0;
        for(int i = 0; i < 8; i++)
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
    std::array<float, 10> coefficients;
    std::array<float, 4> delay;
    int order;
};

}
}
