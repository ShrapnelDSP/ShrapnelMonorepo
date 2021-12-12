#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "string.h"

/* TODO generate mocks with CppUMock */
BaseType_t xQueueReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait )
{
    (void) xQueue;
    (void) xTicksToWait;

    char dummy[] = "Hello World";
    memcpy(pvBuffer, dummy, sizeof(dummy));
    return pdTRUE;
}
