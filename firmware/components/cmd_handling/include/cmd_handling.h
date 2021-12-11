#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stddef.h>

void cmd_init(QueueHandle_t q, size_t a_message_size);

#ifdef __cplusplus
}
#endif
