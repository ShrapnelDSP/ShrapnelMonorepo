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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "fast_convolution.h"
#include <complex>

void _esp_error_check_failed(esp_err_t rc,
                             const char *file,
                             int line,
                             const char *function,
                             const char *expression)
{
    printf("ESP_ERROR_CHECK failed: esp_err_t 0x%x", rc);
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
    printf(" (%s)", esp_err_to_name(rc));
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP
    printf(" at %p\n", __builtin_return_address(0));
    printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n",
           file,
           line,
           function,
           expression);
    abort();
}

void profiling_mark_stage(unsigned int stage) { (void)stage; }

using ::testing::ElementsAre;
using ::testing::FloatEq;

class FastConvolution : public ::testing::Test
{
};

TEST_F(FastConvolution, ComplexMultiply)
{
    using namespace std::complex_literals;

    std::array<std::complex<float>, 8> in_a{
        2.f + 3if,
        1.f + 0if,
        0.f + 2if,
        1.f + 2if,
        -1.f + 0if,
        0.f + -2if,
        -1.f + -2if,
    };

    std::array<std::complex<float>, 8> in_b{
        7.f + 21if,
        1.f + 1if,
        1.f + 1if,
        1.f + 1if,
        1.f + 1if,
        1.f + 1if,
        1.f + 1if,
    };

    std::array<std::complex<float>, 8> out_uut;

    shrapnel::dsp::FastConvolution<16, 16>::complex_multiply(
        reinterpret_cast<float *>(in_a.data()),
        reinterpret_cast<float *>(in_b.data()),
        reinterpret_cast<float *>(out_uut.data()));

    for(std::size_t i = 0; i < 8; i++)
    {
        auto ref = i == 0 ? in_a[0].real() * in_b[0].real() +
                                1if * (in_a[0].imag() * in_b[0].imag())
                          : in_a[i] * in_b[i];

        EXPECT_FLOAT_EQ(ref.real(), out_uut[i].real())
            << "i: " << i << " in_a: " << in_a[i] << " in_b: " << in_b[i];
        EXPECT_FLOAT_EQ(ref.imag(), out_uut[i].imag())
            << "i: " << i << " in_a: " << in_a[i] << " in_b: " << in_b[i];
    }
}

TEST_F(FastConvolution, ImpulseZeroDelayIsIdentity)
{
    std::array<float, 8> input_a{};
    std::array<float, 8> input_b{};

    input_b[0] = 1;

    input_a[0] = 1;
    input_a[1] = 2;
    input_a[2] = 3;
    input_a[3] = 4;
    input_a[4] = 5;
    input_a[5] = 6;
    input_a[6] = 7;
    input_a[7] = 8;

    std::array<float, 8> out{};

    shrapnel::dsp::FastConvolution<8, 8> uut(input_a);

    uut.process(input_b, out);

    EXPECT_THAT(out,
                ElementsAre(FloatEq(1),
                            FloatEq(2),
                            FloatEq(3),
                            FloatEq(4),
                            FloatEq(5),
                            FloatEq(6),
                            FloatEq(7),
                            FloatEq(8)));
}

TEST_F(FastConvolution, ImpulseNonZeroDelay)
{
    std::array<float, 8> input_a{};
    std::array<float, 8> input_b{};

    input_b[1] = 1;

    input_a[0] = 1;
    input_a[1] = 2;
    input_a[2] = 3;
    input_a[3] = 4;
    input_a[4] = 5;
    input_a[5] = 6;
    input_a[6] = 7;
    input_a[7] = 8;

    std::array<float, 8> out{};

    shrapnel::dsp::FastConvolution<8, 8> uut(input_a);

    uut.process(input_b, out);

    EXPECT_THAT(out,
                ElementsAre(FloatEq(8),
                            FloatEq(1),
                            FloatEq(2),
                            FloatEq(3),
                            FloatEq(4),
                            FloatEq(5),
                            FloatEq(6),
                            FloatEq(7)));
}

TEST_F(FastConvolution, IsCommutative)
{
    std::array<float, 8> input_a{};
    std::array<float, 8> input_b{};

    input_a[0] = 1;
    input_a[1] = 2;
    input_a[2] = 3;
    input_a[3] = 4;
    input_a[4] = 5;
    input_a[5] = 6;
    input_a[6] = 7;
    input_a[7] = 8;

    input_b[0] = 9;
    input_b[1] = 10;
    input_b[2] = 11;
    input_b[3] = 12;
    input_b[4] = 13;
    input_b[5] = 14;
    input_b[6] = 15;
    input_b[7] = 16;

    std::array<float, 8> out_a{};
    std::array<float, 8> out_b{};

    shrapnel::dsp::FastConvolution<8, 8> uut_a(input_a);
    shrapnel::dsp::FastConvolution<8, 8> uut_b(input_b);

    uut_a.process(input_b, out_a);
    uut_b.process(input_a, out_b);

    for(std::size_t i = 0; i < 8; i++)
    {
        EXPECT_FLOAT_EQ(out_a[i], out_b[i]);
    }
}

TEST_F(FastConvolution, IsLinear)
{
    std::array<float, 8> input_a{};
    std::array<float, 8> input_b{};

    input_b[0] = 1;
    input_b[1] = 1;

    input_a[0] = 1;
    input_a[1] = 2;
    input_a[2] = 3;
    input_a[3] = 4;
    input_a[4] = 5;
    input_a[5] = 6;
    input_a[6] = 7;
    input_a[7] = 8;

    std::array<float, 8> out{};

    shrapnel::dsp::FastConvolution<8, 8> uut(input_a);

    uut.process(input_b, out);

    EXPECT_THAT(out,
                ElementsAre(FloatEq(1 + 8),
                            FloatEq(2 + 1),
                            FloatEq(3 + 2),
                            FloatEq(4 + 3),
                            FloatEq(5 + 4),
                            FloatEq(6 + 5),
                            FloatEq(7 + 6),
                            FloatEq(8 + 7)));
}
