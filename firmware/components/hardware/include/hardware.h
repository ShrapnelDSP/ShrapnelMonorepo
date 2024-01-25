/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"

#define HW_BREADBOARD 0
#define HW_PCB_REV1 1

#define HARDWARE HW_PCB_REV1

#if HARDWARE == HW_BREADBOARD

#define PROFILING_GPIO GPIO_NUM_23

#define GPIO_I2C_SDA GPIO_NUM_13
#define GPIO_I2C_SCL GPIO_NUM_12

#define I2S_NUM (1) //PCM3060 pin
/* The only other option conflicts with the UART0 used for debugging */
#define I2S_MCK_IO (GPIO_NUM_0) //6/9
#define I2S_BCK_IO (GPIO_NUM_2) //5/10
#define I2S_WS_IO (GPIO_NUM_4)  //4/11
#define I2S_DO_IO (GPIO_NUM_5)  //12
#define I2S_DI_IO (GPIO_NUM_19) //3

#elif HARDWARE == HW_PCB_REV1

#define PROFILING_GPIO GPIO_NUM_15

#define GPIO_I2C_SDA GPIO_NUM_22
#define GPIO_I2C_SCL GPIO_NUM_23

#define I2S_NUM (1)                   //PCM3060 pin
/* The only other option conflicts with the UART0 used for debugging */
#define I2S_MCK_IO (GPIO_NUM_0)       //6
#define I2S_BCK_IO (GPIO_NUM_18)      //5
#define I2S_WS_IO (GPIO_NUM_19)       //4
#define I2S_DO_IO (GPIO_NUM_2)        //12
#define I2S_DI_IO (GPIO_NUM_21)       //3
#define GPIO_CODEC_NRESET GPIO_NUM_27 //15

#else
#error Invalid hardware define
#endif

#define I2C_NUM I2C_NUM_0
#define UART_NUM_MIDI UART_NUM_1
#define GPIO_NUM_MIDI GPIO_NUM_26
