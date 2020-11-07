#include "process.h"
#include "dsps_biquad.h"
#include "float_convert.h"

#include "driver/gpio.h"

#include <assert.h>

extern float gain;
extern gpio_num_t g_profiling_gpio;

void process_samples(int32_t *buf, size_t buf_len)
{
    assert(g_profiling_gpio != -1 && "I2S task has not been initialised");

    gpio_set_level(g_profiling_gpio, 1);
    for(int i = 0; i < buf_len; i++)
    {
        float f = buf[i]/(float)INT32_MAX;

        f *= gain;

        buf[i] = float_to_int32(f);
    }
    gpio_set_level(g_profiling_gpio, 0);
}
