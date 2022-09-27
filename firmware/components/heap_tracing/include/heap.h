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

#include "esp_heap_trace.h"

template<std::size_t N>
class EspHeapTracing {
    public:
    static EspHeapTracing& instance()
    {
        static EspHeapTracing instance;
        return instance;
    }

    void start(void)
    {
        ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_ALL));
    }

    void stop(void)
    {
        ESP_ERROR_CHECK(heap_trace_stop());
        heap_trace_dump();
    }

    private:
    EspHeapTracing() {
        ESP_ERROR_CHECK(heap_trace_init_standalone(record_buffer, N));
    }

    heap_trace_record_t record_buffer[N];
};



template<std::size_t N>
class ScopedHeapTracing {
    public:
    ScopedHeapTracing()
    {
        auto tracing = EspHeapTracing<N>::instance();
        tracing.start();
    }

    ~ScopedHeapTracing()
    {
        auto tracing = EspHeapTracing<N>::instance();
        tracing.stop();
    }
};
