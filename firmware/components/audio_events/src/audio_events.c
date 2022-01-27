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

#include "audio_events.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define TAG "audio_events"

EventGroupHandle_t g_audio_event_group;

static const char *clipping_message = "{\"event\": \"Output Clipped\"}";

#define CLIPPING_REPEAT_TICK (1000/portTICK_PERIOD_MS)
static void audio_event_task(void *parameters)
{
    int bits;
    TickType_t last_clipping_tick = 0;

    while(1)
    {
        bits = xEventGroupWaitBits(g_audio_event_group, 0xFFFFFF, true, false, portMAX_DELAY);

        if(bits & AUDIO_EVENT_OUTPUT_CLIPPED)
        {

            /* Clipping messages are throttled to not send more data then
             * needed */
            if((xTaskGetTickCount() - last_clipping_tick) > CLIPPING_REPEAT_TICK)
            {
                audio_event_send_callback(clipping_message, -1);

                ESP_LOGI(TAG, "Sent clipping message");
                last_clipping_tick = xTaskGetTickCount();
            }
            //clear the bit we just checked
            bits &= (~AUDIO_EVENT_OUTPUT_CLIPPED);
        }

        if(bits)
        {
            ESP_LOGE(TAG, "Some bits were not handled %x", bits);
        }
    }
}

esp_err_t audio_event_init(void)
{
    g_audio_event_group = xEventGroupCreate();

    if(g_audio_event_group == NULL)
    {
        ESP_LOGE(TAG, "Failed to create event group");
        return ESP_FAIL;
    }

    if(pdPASS != xTaskCreate(audio_event_task, "audio event", 2000, NULL, 4, NULL))
    {
        ESP_LOGE(TAG, "Failed to create task");
        return ESP_FAIL;
    }

    return ESP_OK;
}
