#include "timer_impl.h"
#include "os/timer.h"

namespace shrapnel::os {

Timer::impl::impl(const char *pcTimerName,
                  TickType_t xTimerPeriod,
                  const UBaseType_t uxAutoReload,
                  std::optional<etl::delegate<void(void)>> callback)
    : callback(callback)
{
    timers.enable(true);
    id = timers.register_timer(
        Callback::create<Timer::impl, &Timer::impl::do_callback>(*this),
        xTimerPeriod,
        uxAutoReload);
}

BaseType_t Timer::impl::is_active() const { return is_running; }

BaseType_t Timer::impl::start(TickType_t xBlockTime)
{
    is_running = true;
    return timers.start(id);
}

BaseType_t Timer::impl::stop(TickType_t xBlockTime)
{
    is_running = false;
    return timers.stop(id);
}

void Timer::impl::tick(uint32_t tick_count) { timers.tick(tick_count); }

etl::callback_timer_atomic<254> Timer::impl::timers;

} // namespace shrapnel::os
