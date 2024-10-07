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

#include <math.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INT24_MAX 8388607

static inline int32_t float_to_int32(float f)
{
    f *= INT24_MAX;
    if(f > 0.f)
    {
        return (int32_t)(f + 0.5f) << 8;
    }
    else
    {
        return (int32_t)(f - 0.5f) << 8;
    }
}

#ifdef __cplusplus
}
#endif
