#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef int QueueHandle_t;

QueueHandle_t xQueueCreate( size_t uxQueueLength, size_t uxItemSize );
BaseType_t xQueueSendToBack( QueueHandle_t xQueue, void *pvItemToQueue, TickType_t xTicksToWait );
BaseType_t xQueueReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait );

#ifdef __cplusplus
}
#endif
