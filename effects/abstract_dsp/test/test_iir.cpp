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

#include "iir_universal.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

using ::testing::FloatNear;
using ::testing::Pointwise;

TEST(iir, unity_filter_response)
{
    /* create a coefficient array that corresponds to an FIR filter with an
     * impulse response of [ 1, 0, 0 ...] */

    /* 1 + 0 * z**-1
     * -------------
     * 1 + 0 * z**-1
     */
    float coeff[] = {1, 0, 1, 0};
    float impulse[] = {1, 0, 0, 0};
    float output[ARRAY_SIZE(impulse)] = {0};
    float w[ARRAY_SIZE(impulse)] = {0};
    iir_process(
        impulse, output, ARRAY_SIZE(impulse), coeff, w, ARRAY_SIZE(coeff));
    EXPECT_THAT(output, Pointwise(FloatNear(1e-6f), impulse));
}

TEST(iir, delay_filter_response)
{
    /* create a coefficient array that corresponds to an FIR filter with an
     * impulse response of [ 0, 1, 0, 0 ...] */

    /* 0 + 1 * z**-1
     * -------------
     * 1 + 0 * z**-1
     */
    float coeff[] = {0, 1, 1, 0};
    float impulse[] = {1, 0, 0, 0};
    float expected[] = {0, 1, 0, 0};
    float output[ARRAY_SIZE(impulse)] = {0};
    float w[ARRAY_SIZE(impulse)] = {0};
    iir_process(
        impulse, output, ARRAY_SIZE(impulse), coeff, w, ARRAY_SIZE(coeff));
    EXPECT_THAT(output, Pointwise(FloatNear(1e-6f), expected));
}

TEST(iir, div_by_two_unity_filter_response)
{
    /* create a coefficient array that corresponds to an FIR filter with an
     * impulse response of [ 1/2, 0, 0, 0 ...] */

    /* 1   + 0 * z**-1
     * -------------
     * 1/2 + 0 * z**-1
     */
    float coeff[] = {0.5, 0, 1, 0};
    float impulse[] = {1, 0, 0, 0};
    float expected[] = {0.5, 0, 0, 0};
    float output[ARRAY_SIZE(impulse)] = {0};
    float w[ARRAY_SIZE(impulse)] = {0};
    iir_process(
        impulse, output, ARRAY_SIZE(impulse), coeff, w, ARRAY_SIZE(coeff));
    EXPECT_THAT(output, Pointwise(FloatNear(1e-6f), expected));
}

TEST(iir, convolution_filter_response)
{
    /* create a coefficient array that corresponds to an FIR filter with an
     * impulse response of [ 1, 1, 0, 0 ...] */

    /* 1 + 1 * z**-1
     * -------------
     * 1 + 0 * z**-1
     */
    float coeff[] = {1, 1, 1, 0};
    float input[] = {1, 1, 0, 0};
    float expected[] = {1, 2, 1, 0};
    float output[ARRAY_SIZE(input)] = {0};
    float w[ARRAY_SIZE(input)] = {0};
    iir_process(input, output, ARRAY_SIZE(input), coeff, w, ARRAY_SIZE(coeff));
    EXPECT_THAT(output, Pointwise(FloatNear(1e-6f), expected));
}

TEST(iir, third_order_impulse)
{
    /* This filter and expected impulse response can be created using the
     * following python script 
     * 
     * import scipy.signal as s
     * b, a = s.iirfilter(3, 0.25, btype='lowpass')
     * for c in list(b) + list(a):
     *     print(c, end=', ')
     *
     * impulse = s.unit_impulse(10)
     * response = s.lfilter(b, a, impulse)
     * */

    float coeff[] = {0.03168934384971104,
                     0.09506803154913313,
                     0.09506803154913313,
                     0.03168934384971104,
                     1.0,
                     -1.459029062228061,
                     0.9103690002900686,
                     -0.19782518726431944};
    float input[] = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float expected[] = {0.03168934384971104,
                        0.1413037051887996,
                        0.2723852477397881,
                        0.3067377739755946,
                        0.22752167297501763,
                        0.10660083515409813,
                        0.009085496154783684,
                        -0.03878057523175791,
                        -0.043764810187733885,
                        -0.02675215648417989};
    float output[ARRAY_SIZE(input)] = {0};
    float w[ARRAY_SIZE(coeff) / 2] = {0};
    iir_process(input, output, ARRAY_SIZE(input), coeff, w, ARRAY_SIZE(coeff));
    EXPECT_THAT(output, Pointwise(FloatNear(1e-6f), expected));
}

TEST(iir, third_order_response)
{
    /* Created similar to above, but with a different input signal */
    float coeff[] = {0.03168934384971104,
                     0.09506803154913313,
                     0.09506803154913313,
                     0.03168934384971104,
                     1.0,
                     -1.459029062228061,
                     0.9103690002900686,
                     -0.19782518726431944};
    float input[] = {0.0,
                     0.5877852522924731,
                     0.9510565162951535,
                     0.9510565162951536,
                     0.5877852522924732,
                     1.2246467991473532e-16,
                     -0.587785252292473,
                     -0.9510565162951535,
                     -0.9510565162951536,
                     -0.5877852522924734,
                     -2.4492935982947064e-16};
    float expected[] = {0.0,
                        0.018626528969685335,
                        0.11319459096964525,
                        0.32463019812532173,
                        0.5923640432356142,
                        0.7675688414948406,
                        0.7122468297940586,
                        0.3902110581408493,
                        -0.10366672513274929,
                        -0.5836716529057598,
                        -0.8564583705870229};
    float output[ARRAY_SIZE(input)] = {0};
    float w[ARRAY_SIZE(coeff) / 2] = {0};
    iir_process(input, output, ARRAY_SIZE(input), coeff, w, ARRAY_SIZE(coeff));
    EXPECT_THAT(output, Pointwise(FloatNear(1e-6f), expected));
}
