#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef enum
{
    AUDIO_EVENT_OUTPUT_CLIPPED = (1 << 0),
} audio_event_t;

extern EventGroupHandle_t g_audio_event_group;
