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

#include "fast_convolution.h"
#include "fast_fir.h"

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

using ::testing::ElementsAre;
using ::testing::FloatEq;

class FastFirIntegration : public ::testing::Test {};

TEST_F(FastFirIntegration, ImpulseZeroDelayIsIdentity)
{
    std::array<float, 4> input{
        1,
    };

    std::array<float, 4> coefficients{
        1,
        2,
        3,
        4,
    };

    shrapnel::dsp::FastConvolution<8> convolution;
    shrapnel::dsp::FastFir<4, 8, 4, shrapnel::dsp::FastConvolution<8>> uut(coefficients, convolution);

    uut.process(input.data());

    EXPECT_THAT(input, ElementsAre(
                FloatEq(1),
                FloatEq(2),
                FloatEq(3),
                FloatEq(4)
                ));
}

TEST_F(FastFirIntegration, DISABLED_ImpulseNonZeroDelay)
{
    std::array<float, 4> input_a{};
    std::array<float, 4> input_b{};

    input_b[1] = 1;

    input_a[0] = 1;
    input_a[1] = 2;
    input_a[2] = 3;
    input_a[3] = 4;

    std::array<float, 4> out{};

    shrapnel::dsp::FastConvolution<4> uut;

    uut.process(input_a, input_b, out);

    EXPECT_THAT(out, ElementsAre(
                FloatEq(0),
                FloatEq(1),
                FloatEq(2),
                FloatEq(3)
                ));
}

TEST_F(FastFirIntegration, DISABLED_IsCommutative)
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

    shrapnel::dsp::FastConvolution<4> uut;

    uut.process(input_a, input_b, out_a);
    uut.process(input_b, input_a, out_b);

    for(int i = 0; i < 4; i++)
    {
        EXPECT_FLOAT_EQ(out_a[i], out_b[i]);
    }
}

TEST_F(FastFirIntegration, DISABLED_IsLinear)
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

    shrapnel::dsp::FastConvolution<4> uut;

    uut.process(input_a, input_b, out);

    EXPECT_THAT(out, ElementsAre(
                FloatEq(1 + 0),
                FloatEq(2 + 1),
                FloatEq(3 + 2),
                FloatEq(4 + 3)
                ));
}
