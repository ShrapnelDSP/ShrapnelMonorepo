#include "i2s.h"
#include "process.h"
#include "dsps_biquad.h"
#include "float_convert.h"
#include "math.h"

#include "driver/gpio.h"

#include <assert.h>

extern float gain;
extern gpio_num_t g_profiling_gpio;
float fbuf[DMA_BUF_SIZE];

//these approximate the pre EQ from a Boss HM-2 pedal
static float coeff[3][5] = {
    { 1.016224171805848, -1.996244766867333, 0.980170456681741, 
        -1.996244766867333, 0.996394628487589 },
    { 1.048098514125369, -1.946884930731663, 0.914902628855116,
        -1.946884930731663, 0.963001142980485 },
    { 1.260798129602192, -1.896438187817481, 0.674002337997260,
        -1.896438187817481, 0.934800467599452}
};

static float delay_line[3][2];

float waveshape(float x)
{
    float y;
    if (x >= -1 && x < -0.08905) {
        y = -3.0 / 4.0 *
            (1 - powf(1 - (fabs(x) - 0.032847), 12) +
             1.0 / 3.0 * (fabs(x) - 0.032847)) +
            0.01;
    } else if (x >= -0.08905 && x < 0.320018) {
        y = -6.153 * powf(x, 2) + 3.9375 * x;
    } else {
        y = 0.630035;
    }
    return y;
}

void process_samples(int32_t *buf, size_t buf_len)
{
    assert(g_profiling_gpio != -1 && "I2S task has not been initialised");

    gpio_set_level(g_profiling_gpio, 1);
    for(int i = 0; i < buf_len; i++)
    {
        fbuf[i] = buf[i]/(float)INT32_MAX;
        fbuf[i] *= gain;
    }

    for(int i = 0; i < 3; i++)
    {
        dsps_biquad_f32_ae32(fbuf, fbuf, buf_len, coeff[i], delay_line[i]);
    }

    for(int i = 0; i < buf_len; i++)
    {
        buf[i] = float_to_int32(fbuf[i]);
    }

    gpio_set_level(g_profiling_gpio, 0);
}
