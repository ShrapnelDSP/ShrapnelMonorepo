#include "os/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

namespace shrapnel::os {

extern "C" void timer_callback(TimerHandle_t a_timer);

struct Timer::impl
{
    impl(const char *pcTimerName,
         TickType_t xTimerPeriod,
         const UBaseType_t uxAutoReload,
         std::optional<etl::delegate<void(void)>> callback);

    ~impl();

    BaseType_t is_active() const;
    BaseType_t start(TickType_t xBlockTime);
    BaseType_t stop(TickType_t xBlockTime);

    friend void shrapnel::os::timer_callback(TimerHandle_t a_timer);
    TimerHandle_t timer;
    std::optional<Callback> callback;
};

}