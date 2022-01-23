#pragma once

#include <stdint.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INT24_MAX 8388607

static inline int32_t float_to_int32(float f)
{
    f *= INT24_MAX;
    if(f > 0.f) {
        return (int32_t)(f + 0.5f) << 8;
    }
    else {
        return (int32_t)(f - 0.5f) << 8;
    }
}

#ifdef __cplusplus
}
#endif
