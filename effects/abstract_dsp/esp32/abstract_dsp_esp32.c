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

#include "dsps_biquad_gen.h"
#include "dsps_mul.h"

#include "malloc.h"
#include "string.h"
#include "assert.h"

struct dspal_iir {
    float *coeffs;
    float *delay;
    size_t order;
};

dspal_err_t dspal_biquad_design_lowpass(float *coeffs, float f, float q_factor)
{
    if(ESP_OK != dsps_biquad_gen_lpf_f32(coeffs, f, q_factor))
    {
        return DSPAL_FAIL;
    }

    memmove(&coeffs[4], &coeffs[3], 2*sizeof(float));
    coeffs[3] = 1.f;

    return DSPAL_OK;
}

void dspal_multiply(const float *in1, const float *in2, float *out, size_t buf_size)
{
    dsps_mul_f32_ae32(in1, in2, out, buf_size, 1, 1, 1);
}
