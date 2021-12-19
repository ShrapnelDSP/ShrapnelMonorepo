#include "queue.h"

#include <cassert>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

namespace shrapnel {

template<typename T>
Queue<T>::Queue(int number_of_elements)
{
    handle = xQueueCreate(number_of_elements, sizeof(T));
    assert(handle);
}

template<typename T>
BaseType_t Queue<T>::receive(T *out, TickType_t time_to_wait)
{
    return xQueueReceive(handle, out, time_to_wait);
}

}
