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
        // make sure heap tracing has been initialised. This is a singleton, so
        // initialisation will only run once
        EspHeapTracing<N>::instance();

        ESP_ERROR_CHECK(heap_trace_start(HEAP_TRACE_ALL));
    }

    ~ScopedHeapTracing()
    {
        ESP_ERROR_CHECK(heap_trace_stop());
        heap_trace_dump();
    }
};
