#include <stdint.h>
#include <math.h>

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
