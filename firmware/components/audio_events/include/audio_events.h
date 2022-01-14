#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

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
