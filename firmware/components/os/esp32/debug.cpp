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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "os_debug"

void debug_dump_task_list()
{
#if configUSE_TRACE_FACILITY && configUSE_STATS_FORMATTING_FUNCTIONS
    constexpr size_t characters_per_task = 40;
    constexpr size_t approximate_task_count = 20;
    char buffer[characters_per_task * approximate_task_count + 1] = {0};
    vTaskList(buffer);
    // crash if the buffer was overflowed
    assert(buffer[sizeof(buffer) - 1] == '\0');
    ESP_LOGI(TAG, "Task list:\n%s", buffer);
#endif
}

