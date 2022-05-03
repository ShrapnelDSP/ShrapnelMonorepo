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


void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    printf("ESP_ERROR_CHECK failed: esp_err_t 0x%x", rc);
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
    printf(" (%s)", esp_err_to_name(rc));
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP
    printf(" at %p\n", __builtin_return_address(0));
    printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n", file, line, function, expression);
    abort();
}

void profiling_mark_stage(unsigned int stage) {(void)stage;}

using ::testing::ElementsAre;
using ::testing::FloatEq;

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

    std::array<std::complex<float>, 8> out_uut;

    shrapnel::dsp::FastConvolution<8, 8>::complex_multiply(in_a, in_b, out_uut);

    for(std::size_t i = 0; i < 8; i++)
    {
        auto ref = in_a[i] * in_b[i];

        EXPECT_FLOAT_EQ(ref.real(), out_uut[i].real()) <<
            "i: " << i << " in_a: " << in_a[i] << " in_b: " << in_b[i];
        EXPECT_FLOAT_EQ(ref.imag(), out_uut[i].imag()) <<
            "i: " << i << " in_a: " << in_a[i] << " in_b: " << in_b[i];
    }
}

TEST_F(FastConvolution, ImpulseZeroDelayIsIdentity)
{
    std::array<float, 4> input_a{};
    std::array<float, 4> input_b{};

    input_b[0] = 1;

    input_a[0] = 1;
    input_a[1] = 2;
    input_a[2] = 3;
    input_a[3] = 4;

    std::array<float, 4> out{};

    shrapnel::dsp::FastConvolution<4, 4> uut(input_a);

    uut.process(input_b, out);

    EXPECT_THAT(out, ElementsAre(
                FloatEq(1),
                FloatEq(2),
                FloatEq(3),
                FloatEq(4)
                ));
}

TEST_F(FastConvolution, ImpulseNonZeroDelay)
{
    std::array<float, 4> input_a{};
    std::array<float, 4> input_b{};

    input_b[1] = 1;

    input_a[0] = 1;
    input_a[1] = 2;
    input_a[2] = 3;
    input_a[3] = 4;

    std::array<float, 4> out{};

    shrapnel::dsp::FastConvolution<4, 4> uut(input_a);

    uut.process(input_b, out);

    EXPECT_THAT(out, ElementsAre(
                FloatEq(4),
                FloatEq(1),
                FloatEq(2),
                FloatEq(3)
                ));
}

TEST_F(FastConvolution, IsCommutative)
{
    std::array<float, 4> input_a{};
    std::array<float, 4> input_b{};

    input_a[0] = 1;
    input_a[1] = 2;
    input_a[2] = 3;
    input_a[3] = 4;

    input_b[0] = 5;
    input_b[1] = 6;
    input_b[2] = 7;
    input_b[3] = 8;

    std::array<float, 4> out_a{};
    std::array<float, 4> out_b{};

    shrapnel::dsp::FastConvolution<4, 4> uut_a(input_a);
    shrapnel::dsp::FastConvolution<4, 4> uut_b(input_b);

    uut_a.process(input_b, out_a);
    uut_b.process(input_a, out_b);

    for(std::size_t i = 0; i < 4; i++)
    {
        EXPECT_FLOAT_EQ(out_a[i], out_b[i]);
    }
}

TEST_F(FastConvolution, IsLinear)
{
    std::array<float, 4> input_a{};
    std::array<float, 4> input_b{};

    input_b[0] = 1;
    input_b[1] = 1;

    input_a[0] = 1;
    input_a[1] = 2;
    input_a[2] = 3;
    input_a[3] = 4;

    std::array<float, 4> out{};

    shrapnel::dsp::FastConvolution<4, 4> uut(input_a);

    uut.process(input_b, out);

    EXPECT_THAT(out, ElementsAre(
                FloatEq(1 + 4),
                FloatEq(2 + 1),
                FloatEq(3 + 2),
                FloatEq(4 + 3)
                ));
}
