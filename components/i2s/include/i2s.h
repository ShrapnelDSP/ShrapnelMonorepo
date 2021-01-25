#ifndef I2S_H
#define I2S_H

#include "esp_err.h"
#include "driver/gpio.h"

#define DMA_BUF_SIZE    (64)
#define SAMPLE_RATE     (48000)

esp_err_t i2s_setup(gpio_num_t profiling_gpio);

void i2s_set_volume(float a);
void i2s_set_pedal_gain(float a);
void i2s_set_amp_gain(float a);

#endif //I2S_H
