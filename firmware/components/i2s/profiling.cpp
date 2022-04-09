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

#include "profiling.h"

#include "i2s.h"
#include "esp_log.h"
#include "hal/cpu_hal.h"
#include "freertos/task.h"
#include <assert.h>

#define TAG "profiling"

#define NUMBER_OF_STAGES 25

static uint32_t start_cycles = 0;
static uint32_t stage_cycles[NUMBER_OF_STAGES] = {};
SemaphoreHandle_t profiling_mutex;
static bool got_semaphore = false;
static int cpu_freq_mhz;

void profiling_start(void)
{
    if(xSemaphoreTake(profiling_mutex, 0))
    {
        got_semaphore = true;
        start_cycles = cpu_hal_get_cycle_count();
    }
}

void profiling_mark_stage(unsigned int stage)
{
    assert(stage < NUMBER_OF_STAGES);

    if(got_semaphore)
    {
        stage_cycles[stage] = cpu_hal_get_cycle_count();
    }
}

void profiling_stop(void)
{
    if(got_semaphore)
    {
        xSemaphoreGive(profiling_mutex);
        got_semaphore = false;
    }
}

static double cycles_to_percent(int64_t cycles)
{
    float cycles_per_second = cpu_freq_mhz * 1e6f;

    float ratio = cycles /
        ((float)DMA_BUF_SIZE / SAMPLE_RATE) /
        (cycles_per_second);

    return (double)(100 * ratio);
}

static int64_t cycles_to_us(int64_t cycles)
{
    return cycles / cpu_freq_mhz;
}

void i2s_profiling_task(void *param)
{
    cpu_freq_mhz = ets_get_cpu_frequency();
    ESP_LOGI(TAG, "%d", cpu_freq_mhz);

    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        if(xSemaphoreTake(profiling_mutex, 100 / portTICK_PERIOD_MS))
        {
            ESP_LOGI(TAG, " ========= \n");

            int64_t total_cycles = 0;
            for(int i = 0; i < NUMBER_OF_STAGES; i++)
            {
                //if the stage has not been assigned, stop printing
                if(stage_cycles[i] == 0)
                {
                    break;
                }

                int64_t current_stage_cycles = stage_cycles[i] - ((i == 0) ? start_cycles : stage_cycles[i - 1]);
                ESP_LOGI(TAG, "Stage %3d processing took %6lld cycles %4lld us (%03.1f %%)",
                        i,
                        current_stage_cycles,
                        cycles_to_us(current_stage_cycles),
                        cycles_to_percent(current_stage_cycles));

                total_cycles += current_stage_cycles;
            }

            ESP_LOGI(TAG, "Total processing took     %6lld cycles %4lld us (%03.1f %%)",
                    total_cycles,
                    cycles_to_us(total_cycles),
                    cycles_to_percent(total_cycles));

            ESP_LOGI(TAG, " ========= \n");

            xSemaphoreGive(profiling_mutex);
        }
        else
        {
            ESP_LOGE(TAG, "%s failed to take semaphore", __FUNCTION__);
        }
    }
}
