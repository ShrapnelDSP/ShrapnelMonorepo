/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "dsps_biquad.h"
#include "iir.h"
#include "iir_universal.h"
#include <array>

extern "C" void dspal_esp32_3rd_order_iir(
    const float *input, float *output, uint32_t len, float *coeffs, float *w);

namespace shrapnel {
namespace dsp {

class IirFilter final : public IirFilterBase
{
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
            dsps_biquad_f32_ae32(
                in, out, buf_size, coefficients.data(), delay.data());
        }
        else if(order == 3)
        {
            dspal_esp32_3rd_order_iir(
                in, out, buf_size, coefficients.data(), delay.data());
        }
        else
        {
            iir_process(in,
                        out,
                        buf_size,
                        coefficients.data(),
                        delay.data(),
                        order * 2 + 2);
        }
    }

    void reset(void) override { delay.fill(0); }

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

        // normalise coefficients to [b0, b1, b2, b3, -a1, -a2, -a3]
        for(int i = 0; i < new_coefficients.size() - 1; i++)
        {
            auto coeff_i = i < 4 ? i : i + 1;
            coefficients[i] = new_coefficients[coeff_i] / new_coefficients[4];
            if(i >= 4)
            {
                coefficients[i] *= -1;
            }
        }
    }

    void set_coefficients(std::array<float, 10> new_coefficients) override
    {
        order = 4;
        for(int i = 0; i < new_coefficients.size(); i++)
        {
            coefficients[i] = new_coefficients[i] / new_coefficients[5];
        }
    }

private:
    std::array<float, 10> coefficients;
    std::array<float, 4> delay;
    int order;
};

} // namespace dsp
} // namespace shrapnel
