#include "i2s.h"
#include "process.h"
#include "dsps_biquad.h"
#include "float_convert.h"
#include "dsps_fir.h"
#include "math.h"

#include "driver/gpio.h"

#include <assert.h>

extern float gain;
extern gpio_num_t g_profiling_gpio;
float fbuf[DMA_BUF_SIZE];

#include "speaker_coeffs.h"
float fir_delay_line[sizeof(fir_coeff)/sizeof(fir_coeff[0])];
fir_f32_t fir;

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

static float waveshape(float x)
{
#if 1
    float y;
    if (x >= -1.f && x < -0.08905f) {
        y = -3.0f / 4.0f *
            (1.f - powf(1.f - (fabs(x) - 0.032847f), 12.f) +
             1.f / 3.f * (fabs(x) - 0.032847f)) +
            0.01f;
    } else if (x >= -0.08905f && x < 0.320018f) {
        y = -6.153f * powf(x, 2.f) + 3.9375f * x;
    } else {
        y = 0.630035f;
    }
    return y;
#else
    return tanhf(x);
#endif
}

void process_samples(int32_t *buf, size_t buf_len)
{
    assert(g_profiling_gpio != -1 && "I2S task has not been initialised");

    gpio_set_level(g_profiling_gpio, 1);
    for(int i = 0; i < buf_len; i++)
    {
        fbuf[i] = buf[i]/(float)INT32_MAX;
        fbuf[i] *= gain;
        fbuf[i] = waveshape(fbuf[i]);
    }

    for(int i = 0; i < 3; i++)
    {
        dsps_biquad_f32_ae32(fbuf, fbuf, buf_len, coeff[i], delay_line[i]);
    }
    //dsps_fir_f32_ae32(&fir, fbuf, fbuf, buf_len);

    for(int i = 0; i < buf_len; i++)
    {
        buf[i] = float_to_int32(fbuf[i]);
    }

    gpio_set_level(g_profiling_gpio, 0);
}

esp_err_t process_init()
{
    return dsps_fir_init_f32(&fir, fir_coeff, fir_delay_line,
                             sizeof(fir_coeff) / sizeof(fir_coeff[0]));
}
