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

#include "etl/deque.h"
#include <cassert>
#include <cstdint>
#include <mutex>
#include <queue>
#include <semaphore>
#include <type_traits>

namespace shrapnel {

constexpr uint32_t ms_to_ticks();

enum class queue_error {
    SUCCESS = 0,
    ERROR = 1,
    EMPTY = 2,
    FULL = 3,
};

template <typename T>
class QueueBase
{
public:
    using value_type = T;

    explicit QueueBase(int number_of_elements) { (void)number_of_elements; };

    virtual queue_error receive(T *out, uint32_t time_to_wait) = 0;
    virtual queue_error send(const T *in, uint32_t time_to_wait) = 0;
};

template <typename T, std::size_t MAX_SIZE>
    requires(MAX_SIZE > 0) && (MAX_SIZE < PTRDIFF_MAX)
class Queue final : public QueueBase<T>
{
    using ticks = std::chrono::duration<uint32_t>;

public:
    Queue()
        : QueueBase<T>(MAX_SIZE),
          used_semaphore{0},
          free_semaphore{MAX_SIZE}
    {
    }

    [[nodiscard]] queue_error receive(T *out, uint32_t time_to_wait) override
    {
        // block until an item is available
        bool success = used_semaphore.try_acquire_for(ticks(time_to_wait));
        if(!success)
            return queue_error::EMPTY;

        // Receive the item
        {
            std::scoped_lock lock{mutex};
            *out = queue.front();
            queue.pop();
        }
        free_semaphore.release();
        return queue_error::SUCCESS;
    }

    [[nodiscard]] queue_error send(const T *in, uint32_t time_to_wait) override
    {
        // block until a space is available
        bool success = free_semaphore.try_acquire_for(ticks(time_to_wait));
        if(!success)
            return queue_error::FULL;

        // Add item to queue, and notify receivers
        {
            std::scoped_lock lock{mutex};
            queue.push(*in);
        }
        used_semaphore.release();
        return queue_error::SUCCESS;
    }

private:
    std::counting_semaphore<static_cast<std::ptrdiff_t>(MAX_SIZE)>
        used_semaphore;
    std::counting_semaphore<static_cast<std::ptrdiff_t>(MAX_SIZE)>
        free_semaphore;
    std::queue<T, etl::deque<T, MAX_SIZE>> queue;
    std::mutex mutex;
};

} // namespace shrapnel
