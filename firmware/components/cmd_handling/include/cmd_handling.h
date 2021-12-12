#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stddef.h>

void cmd_init(QueueHandle_t q, size_t a_message_size);
void cmd_task_work(void *context);

#ifdef __cplusplus
}
#endif
