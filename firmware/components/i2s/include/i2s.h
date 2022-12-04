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
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/i2s.h"

#include "audio_param.h"

#define DMA_BUF_SIZE    (512)
#define SAMPLE_RATE     (48000)

extern QueueHandle_t i2s_queue;

/**
 *
 * \param profiling_gpio This pin will go high when samples are
 *                       being processed and return to low once the processing
 *                       is finished.
 *
 */
esp_err_t i2s_setup(gpio_num_t profiling_gpio, shrapnel::parameters::AudioParametersBase *audio_params);

void log_i2s_event(const i2s_event_t &event);
