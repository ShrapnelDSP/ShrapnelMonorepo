#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask )
{
    (void) xTask;

    return 0;
}
