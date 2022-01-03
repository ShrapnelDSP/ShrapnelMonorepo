#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"

#define HW_BREADBOARD 0
#define HW_PCB_REV1 1

#define HARDWARE HW_BREADBOARD

#if HARDWARE == HW_BREADBOARD

#define PROFILING_GPIO GPIO_NUM_23

#define GPIO_I2C_SDA GPIO_NUM_13
#define GPIO_I2C_SCL GPIO_NUM_12

#define I2C_NUM I2C_NUM_0

#define I2S_NUM         (1)             //PCM3060 pin
#define I2S_BCK_IO      (GPIO_NUM_2)    //5/10
#define I2S_WS_IO       (GPIO_NUM_4)    //4/11
#define I2S_DO_IO       (GPIO_NUM_5)    //12
#define I2S_DI_IO       (GPIO_NUM_19)   //3

#elif HARDWARE == HW_PCB_REV1
#error TODO
#else
#error Invalid hardware define
#endif
