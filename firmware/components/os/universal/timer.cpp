#include "os/timer.h"
#include "timer_impl.h"

namespace shrapnel::os {

Timer::Timer(const char *pcTimerName,
             TickType_t xTimerPeriod,
             const UBaseType_t uxAutoReload,
             std::optional<etl::delegate<void(void)>> callback)
{
    p_impl = std::make_unique<impl>(
        pcTimerName, xTimerPeriod, uxAutoReload, callback);
}

Timer::~Timer()
{
}

BaseType_t Timer::is_active() const { return p_impl->is_active(); }

BaseType_t Timer::start(TickType_t xBlockTime)
{
    return p_impl->start(xBlockTime);
}

BaseType_t Timer::stop(TickType_t xBlockTime)
{
    return p_impl->stop(xBlockTime);
}

}