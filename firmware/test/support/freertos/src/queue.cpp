#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

BaseType_t xQueueReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait )
{
#if 0
    return mock().actualCall("xQueueReceive").
            withParameter("xQueue", xQueue).
            withOutputParameter("pvBuffer", pvBuffer).
            withParameter("xTicksToWait", xTicksToWait).
            returnIntValue();
#endif
    return 0;
}
