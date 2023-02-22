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

#include "abstract_dsp.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#include "juce_dsp/juce_dsp.h"
#pragma GCC diagnostic pop
#include <assert.h>

extern "C" {

dspal_err_t dspal_biquad_design_lowpass(float *coeffs, float f, float q_factor)
{
    // TODO check if the frequency is scaled correctly
    auto juce_coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(1, f, q_factor);
    auto raw_coeffs = juce_coeffs->getRawCoefficients();

    assert(2 == juce_coeffs->getFilterOrder());

    // These coefficients are expected to be normalised
    assert(5 == juce_coeffs->coefficients.size());

    memcpy(coeffs, raw_coeffs, 5*sizeof(float));

    return DSPAL_OK;
}

void dspal_multiply(const float *in1, const float *in2, float *out, size_t buf_size)
{
    for(size_t i = 0; i < buf_size; i++)
    {
        out[i] = in1[i] * in2[i];
    }
}

};
