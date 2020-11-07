#include <stdint.h>

static inline int32_t float_to_int32(float f)
{
    return (int32_t) (f * INT32_MAX);
}
