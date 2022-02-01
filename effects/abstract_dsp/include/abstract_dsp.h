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

/** \file
 *
 * Abstraction layer for DSP functionality. This enables the use of use of
 * platform native, and potentially optimised DSP functions in effects in a
 * generic way.
 */

#pragma once

#include <stddef.h>

#include "iir.h"

typedef struct dspal_delayline *dspal_delayline_t;

typedef enum {
    DSPAL_OK,
    DSPAL_FAIL,
} dspal_err_t;

#ifdef __cplusplus
extern "C" {
#endif

dspal_err_t dspal_biquad_design_lowpass(float *coeffs, float f, float q_factor);

void dspal_multiply(const float *in1, const float *in2, float *out, size_t buf_size);

dspal_delayline_t dspal_delayline_create(size_t max_samples);
void dspal_delayline_destroy(dspal_delayline_t delayline);
void dspal_delayline_set_delay(dspal_delayline_t delayline, float delay);
void dspal_delayline_push_sample(dspal_delayline_t delayline, float sample);
float dspal_delayline_pop_sample(dspal_delayline_t delayline);

#ifdef __cplusplus
}
#endif
