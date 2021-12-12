#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "CppUTestExt/MockSupport.h"

BaseType_t xQueueReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait )
{
    return mock().actualCall("xQueueReceive").
            withParameter("xQueue", xQueue).
            withParameter("pvBuffer", pvBuffer).
            withParameter("xTicksToWait", xTicksToWait).
            returnIntValue();
}
