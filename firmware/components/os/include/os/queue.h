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

#include "FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <cassert>
#include <chrono>
#include <cstdint>
#include "etl/deque.h"
#include <semaphore>
#include <type_traits>
#include <queue>
#include <mutex>

namespace shrapnel {

template <typename T>
class QueueBase
{
    public:
    using value_type = T;

    QueueBase(int number_of_elements)
    {
        (void) number_of_elements;
    };

    virtual BaseType_t receive(T *out, TickType_t time_to_wait) = 0;
    virtual BaseType_t send(const T *in, TickType_t time_to_wait) = 0;
};

template <typename T, std::size_t MAX_SIZE> requires (MAX_SIZE > 0) && (MAX_SIZE < PTRDIFF_MAX)
class Queue final: public QueueBase<T>
{
    using ticks = std::chrono::duration<TickType_t>;

    public:
    Queue() : QueueBase<T>(MAX_SIZE), used_semaphore{0}, free_semaphore{MAX_SIZE} {}

    [[nodiscard]] BaseType_t receive(T *out, TickType_t time_to_wait) override
    {
        // block until an item is available
        bool success = used_semaphore.try_acquire_for(ticks(time_to_wait));
        if(!success) return errQUEUE_EMPTY;

        // Receive the item
        {
            std::scoped_lock lock{mutex};
            *out = queue.front();
            queue.pop();
        }
        free_semaphore.release();
        return pdPASS;
    }

    [[nodiscard]] BaseType_t send(const T *in, TickType_t time_to_wait) override
    {
        // block until a space is available
        bool success = free_semaphore.try_acquire_for(ticks(time_to_wait));
        if(!success) return errQUEUE_FULL;

        // Add item to queue, and notify receivers
        {
            std::scoped_lock lock{mutex};
            queue.push(*in);
        }
        used_semaphore.release();
        return pdPASS;
    }

    private:
    std::counting_semaphore<static_cast<std::ptrdiff_t>(MAX_SIZE)> used_semaphore;
    std::counting_semaphore<static_cast<std::ptrdiff_t>(MAX_SIZE)> free_semaphore;
    etl::deque<T, MAX_SIZE> queue_storage;
    std::queue<T, etl::deque<T, MAX_SIZE>> queue;
    std::mutex mutex;
};

}
