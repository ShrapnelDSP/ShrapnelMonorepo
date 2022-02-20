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

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    AUDIO_EVENT_OUTPUT_CLIPPED = (1 << 0),
} audio_event_t;

extern EventGroupHandle_t g_audio_event_group;

esp_err_t audio_event_init(void);

/** \brief Called when an event is triggered.
 *
 * \param message JSON representation of the audio event
 * \param fd The socket file descriptor for the client initiating this event
 *
 * \p fd is used to prevent a client from receiving a message that was
 * triggered by it. The implementation of this callback must ensure that the
 * message is transmitted only to clients where their descriptor is different
 * from \p fd.
 */
void audio_event_send_callback(const char *message, int fd);

#ifdef __cplusplus
}
#endif
