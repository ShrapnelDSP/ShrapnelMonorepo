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

#pragma once

#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Update the parameters of the filter. 
 *
 *  \todo warp the parameter, to make the response more linear
 */
void fmv_update_params(float l, float m, float t);

/** Process a buffer. The buffer is processed in-place.  */
void fmv_process(float *buf, size_t buf_len);

/** Initialise the fmv processor. The bass middle and treble settings are all
 * initialised to 0.5.
 */
esp_err_t fmv_init(float a_sample_rate);

#ifdef __cplusplus
}
#endif
