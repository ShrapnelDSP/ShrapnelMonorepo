/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <cassert>
#include <type_traits>

namespace shrapnel {

template <typename T>
concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

// TODO tuple is not trivially copyable. std::pair with one of the values const
// is. I think we can make the FD const
//
// Maybe we need to redesign and only pass unique ptrs allocated from a memory
// pool? Then it wouldn't matter if the event classes are trivially copyable or
// not.
//
// How does std::queue work around this? Maybe we can just copy construct in
// place, into the queues memory, then again into the receiver memory?
template <typename T> requires TriviallyCopyable<T>
class QueueBase
{
    public:
    using value_type = T;

    QueueBase(int number_of_elements)
    {
        (void) number_of_elements;
    };

    virtual BaseType_t receive(T *out, TickType_t time_to_wait) = 0;
    virtual BaseType_t send(T *out, TickType_t time_to_wait) = 0;
};

template <typename T>
class Queue final: public QueueBase<T>
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
