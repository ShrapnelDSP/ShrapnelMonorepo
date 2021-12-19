#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

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
    Queue(int number_of_elements);
    BaseType_t receive(T *out, TickType_t time_to_wait) override;
    BaseType_t send(T *out, TickType_t time_to_wait) override;

    private:
    QueueHandle_t handle;
};

};
