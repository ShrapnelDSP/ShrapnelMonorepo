#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <cassert>

namespace shrapnel {

template <typename T>
class QueueBase
{
    public:
    QueueBase(int number_of_elements)
    {
        (void) number_of_elements;
    };

    virtual BaseType_t receive(T *out, TickType_t time_to_wait) = 0;
    virtual BaseType_t send(T *out, TickType_t time_to_wait) = 0;
};

template <typename T>
class Queue: public QueueBase<T>
{
    public:
    Queue(int number_of_elements) : QueueBase<T>(number_of_elements)
    {
        handle = xQueueCreate(number_of_elements, sizeof(T));
        assert(handle);
    }

    BaseType_t receive(T *out, TickType_t time_to_wait) override
    {
        return xQueueReceive(handle, out, time_to_wait);
    }

    BaseType_t send(T *out, TickType_t time_to_wait) override
    {
        return xQueueSendToBack(handle, out, time_to_wait);
    }

    private:
    QueueHandle_t handle;
};

}
