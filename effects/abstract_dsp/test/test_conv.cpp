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
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <complex>
#include "fast_convolution.h"

class FastConvolution : public ::testing::Test {};

TEST_F(FastConvolution, ComplexMultiply)
{
    using namespace std::complex_literals;

    std::array<std::complex<float>, 8> in_a{
         0.f +  0if,
         1.f +  0if,
         0.f +  2if,
         1.f +  2if,
        -1.f +  0if,
         0.f + -2if,
        -1.f + -2if,
    };

    std::array<std::complex<float>, 8> in_b{
         1.f + 1if,
         1.f + 1if,
         1.f + 1if,
         1.f + 1if,
         1.f + 1if,
         1.f + 1if,
         1.f + 1if,
    };

    std::array<float, 16> in_a_flat;
    for(int i = 0; i < 16; i++)
    {
        if(!(i % 2))
        {
            in_a_flat[i] = in_a[i/2].real();
        }
        else
        {
            in_a_flat[i] = in_a[i/2].imag();
        }
    }

    std::array<float, 16> in_b_flat;
    for(int i = 0; i < 16; i++)
    {
        if(!(i % 2))
        {
            in_b_flat[i] = in_b[i/2].real();
        }
        else
        {
            in_b_flat[i] = in_b[i/2].imag();
        }
    }

#if 0
    std::cout << "in_a_flat:";
    for(auto &a : in_a_flat)
    {
        std::cout << " " << a;
    }
    std::cout << std::endl;

    std::cout << "in_b_flat:";
    for(auto &b : in_b_flat)
    {
        std::cout << " " << b;
    }
    std::cout << std::endl;
#endif

    std::array<float, 16> out_uut;

    shrapnel::dsp::FastConvolution<8>::complex_multiply(
            in_a_flat, in_b_flat, out_uut);

    for(int i = 0; i < 8; i++)
    {
        auto ref = in_a[i] * in_b[i];

        EXPECT_FLOAT_EQ(ref.real(), out_uut[i*2]) <<
            "i: " << i << " in_a: " << in_a[i] << " in_b: " << in_b[i];
        EXPECT_FLOAT_EQ(ref.imag(), out_uut[i*2 + 1]) <<
            "i: " << i << " in_a: " << in_a[i] << " in_b: " << in_b[i];
    }
}

#if 0
TEST_F(FastConvolution, Functionality)
{
    size_t a_length = 64;
    std::array<float, 128> input_a;

    size_t b_length = 8;
    std::array<float, 128> input_b;

    std::array<float, a_length + b_length - 1> output_ref;
    std::array<float, 128> output_fast;

    FastConvolution<max_N> conv;

    for (size_t la = 1; la < lenA; la++) {
        for (size_t lb = 1; lb < lenB; lb++) {
            for (int i = 0 ; i < lenA ; i++) {
                input_a[i] = (float)rand() / INT32_MAX;
            }
            for (int i = 0 ; i < lenB ; i++) {
                input_b[i] = (float)rand() / INT32_MAX;
            }
            for (int i = 0 ; i < (lenA + lenB  - 1 + 2); i++) {
                output_ref[i] = -1;
                output_fwd[i] = -1;
                output_back[i] = -1;
            }
            dsps_conv_f32(input_a.data(), la,
                          input_b.data(), lb, &output_ref[1]);
            dsps_conv_f32_ansi(inputA, la, inputB, lb, &output_fwd[1]);
            dsps_conv_f32_ansi(inputB, lb, inputA, la, &output_back[1]);
            float max_eps = 0.000001;

            for (size_t i = 0; i < (la + lb + 1); i++)
            {
                if ((fabs(output_ref[i] - output_fwd[i]) > max_eps) ||
                    (fabs(output_ref[i] - output_back[i]) > max_eps) ||
                    (fabs(output_back[i] - output_fwd[i]) > max_eps))
                {
                    ESP_LOGE(TAG, "la=%i, lb=%i, i=%i, "
                                  "ref=%2.3f, fwd=%2.3f, back=%2.3f",
                                  la, lb, i,
                                  output_ref[i], output_fwd[i], output_back[i]);
                }

                TEST_ASSERT_EQUAL(output_ref[i], output_fwd[i]);
                TEST_ASSERT_EQUAL(output_ref[i], output_back[i]);
                TEST_ASSERT_EQUAL(output_back[i], output_fwd[i]);
            }
        }
    }
}
#endif
