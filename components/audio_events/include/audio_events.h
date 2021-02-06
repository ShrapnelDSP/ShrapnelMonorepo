#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

typedef enum
{
    AUDIO_EVENT_OUTPUT_CLIPPED = (1 << 0),
} audio_event_t;

extern EventGroupHandle_t g_audio_event_group;

esp_err_t audio_event_init(QueueHandle_t q);
