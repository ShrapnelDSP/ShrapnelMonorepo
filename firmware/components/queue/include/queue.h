#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

namespace shrapnel {

template <typename T>
class Queue
{
    public:
    virtual BaseType_t receive(char *out, TickType_t time_to_wait) = 0;

    private:
    QueueHandle_t handle;
};

};
