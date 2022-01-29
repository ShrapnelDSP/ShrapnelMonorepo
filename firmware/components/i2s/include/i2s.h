#pragma once

#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/gpio.h"
#include "audio_param.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DMA_BUF_SIZE    (64)
#define SAMPLE_RATE     (48000)

esp_err_t i2s_setup(gpio_num_t profiling_gpio, shrapnel::AudioParametersBase *audio_params);

#ifdef __cplusplus
}
#endif
