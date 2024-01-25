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
#include "fast_fir.h"

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
using ::testing::FloatNear;

class FastFirIntegration : public ::testing::Test
{
};

TEST_F(FastFirIntegration, ImpulseZeroDelayIsIdentity)
{
    std::array<float, 4> input{
        1,
        2,
        3,
        4,
    };

    std::array<float, 1> coefficients{
        1,
    };

    auto convolution =
        std::make_unique<shrapnel::dsp::FastConvolution<16, 1>>(coefficients);
    shrapnel::dsp::FastFir<4, 16, shrapnel::dsp::FastConvolution<16, 1>> uut(
        std::move(convolution));

    uut.process(input);

    EXPECT_THAT(input,
                ElementsAre(FloatEq(1), FloatEq(2), FloatEq(3), FloatEq(4)));
}

TEST_F(FastFirIntegration, ImpulseNonZeroDelay)
{
    std::array<float, 4> input{
        1,
        2,
        3,
        4,
    };

    std::array<float, 2> coefficients{
        0,
        1,
    };

    auto convolution =
        std::make_unique<shrapnel::dsp::FastConvolution<16, 2>>(coefficients);
    shrapnel::dsp::FastFir<4, 16, shrapnel::dsp::FastConvolution<16, 2>> uut(
        std::move(convolution));

    uut.process(input);

    EXPECT_THAT(
        input,
        ElementsAre(FloatNear(0, 1e-6), FloatEq(1), FloatEq(2), FloatEq(3)));
}

TEST_F(FastFirIntegration, IsCommutative)
{
    std::array<float, 4> input_a{
        1,
        2,
        3,
        4,
    };

    std::array<float, 4> input_b{
        5,
        6,
        7,
        8,
    };

    auto convolution_a =
        std::make_unique<shrapnel::dsp::FastConvolution<16, 4>>(input_a);
    shrapnel::dsp::FastFir<4, 16, shrapnel::dsp::FastConvolution<16, 4>> uut_a(
        std::move(convolution_a));

    auto convolution_b =
        std::make_unique<shrapnel::dsp::FastConvolution<16, 4>>(input_b);
    shrapnel::dsp::FastFir<4, 16, shrapnel::dsp::FastConvolution<16, 4>> uut_b(
        std::move(convolution_b));

    uut_a.process(input_b);
    uut_b.process(input_a);

    for(std::size_t i = 0; i < 4; i++)
    {
        EXPECT_FLOAT_EQ(input_a[i], input_b[i]);
    }
}

TEST_F(FastFirIntegration, IsLinear)
{
    std::array<float, 4> input{
        1,
        2,
        3,
        4,
    };

    std::array<float, 2> coefficients{
        1,
        1,
    };

    auto convolution =
        std::make_unique<shrapnel::dsp::FastConvolution<16, 2>>(coefficients);
    shrapnel::dsp::FastFir<4, 16, shrapnel::dsp::FastConvolution<16, 2>> uut(
        std::move(convolution));

    uut.process(input);

    EXPECT_THAT(
        input,
        ElementsAre(
            FloatEq(1 + 0), FloatEq(2 + 1), FloatEq(3 + 2), FloatEq(4 + 3)));
}
