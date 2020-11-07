#ifndef I2S_H
#define I2S_H

#include "esp_err.h"
#include "driver/gpio.h"

esp_err_t i2s_setup(gpio_num_t profiling_gpio);
void i2s_set_gain(float g);

#endif //I2S_H
