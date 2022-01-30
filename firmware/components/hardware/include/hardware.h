#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/i2s.h"

#define HW_BREADBOARD 0
#define HW_PCB_REV1 1

#define HARDWARE HW_PCB_REV1

#if HARDWARE == HW_BREADBOARD

#   define PROFILING_GPIO GPIO_NUM_23

#   define GPIO_I2C_SDA GPIO_NUM_13
#   define GPIO_I2C_SCL GPIO_NUM_12

#   define I2C_NUM I2C_NUM_0

#   define I2S_NUM         (I2S_NUM_1)
                                           //PCM3060 pin
#   define I2S_BCK_IO      (GPIO_NUM_2)    //5/10
#   define I2S_WS_IO       (GPIO_NUM_4)    //4/11
#   define I2S_DO_IO       (GPIO_NUM_5)    //12
#   define I2S_DI_IO       (GPIO_NUM_19)   //3

#elif HARDWARE == HW_PCB_REV1

#   define PROFILING_GPIO GPIO_NUM_15

#   define GPIO_I2C_SDA GPIO_NUM_22
#   define GPIO_I2C_SCL GPIO_NUM_23

#   define I2C_NUM I2C_NUM_0

#   define I2S_NUM         (I2S_NUM_1)
                                           //PCM3060 pin
#   define I2S_BCK_IO      (GPIO_NUM_18)   //5/10
#   define I2S_WS_IO       (GPIO_NUM_19)   //4/11
#   define I2S_DO_IO       (GPIO_NUM_2)    //12
#   define I2S_DI_IO       (GPIO_NUM_21)   //3
#   define GPIO_CODEC_NRESET GPIO_NUM_27    //15

#else
#error Invalid hardware define
#endif
