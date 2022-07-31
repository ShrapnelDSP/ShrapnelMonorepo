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
