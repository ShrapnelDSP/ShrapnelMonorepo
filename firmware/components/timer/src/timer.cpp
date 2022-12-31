#include "timer.h"
#include "freertos/portmacro.h"

namespace shrapnel::os {

extern "C" void timer_callback(TimerHandle_t a_timer)
{
    auto timer =
        reinterpret_cast<shrapnel::os::Timer *>(pvTimerGetTimerID(a_timer));
    timer->callback();
}

Timer::Timer(const char *pcTimerName,
             TickType_t xTimerPeriod,
             const UBaseType_t uxAutoReload,
             etl::delegate<void(void)> callback)
    : callback{callback}
{
    timer = xTimerCreate(
        pcTimerName, xTimerPeriod, uxAutoReload, this, timer_callback);
}

Timer::~Timer()
{
    auto rc = xTimerDelete(timer, portMAX_DELAY);
    assert(rc == pdPASS);
}

BaseType_t Timer::is_active() const { return xTimerIsTimerActive(timer); }

BaseType_t Timer::start(TickType_t xBlockTime)
{
    return xTimerStart(timer, xBlockTime);
}

BaseType_t Timer::stop(TickType_t xBlockTime)
{
    return xTimerStart(timer, xBlockTime);
}

BaseType_t Timer::change_period(TickType_t xNewPeriod, TickType_t xBlockTime)
{
    return xTimerChangePeriod(timer, xNewPeriod, xBlockTime);
}

BaseType_t Timer::reset(TickType_t xBlockTime)
{
    return xTimerReset(timer, xBlockTime);
}

BaseType_t Timer::start_from_isr(BaseType_t &pxHigherPriorityTaskWoken)
{
    return xTimerStartFromISR(timer, &pxHigherPriorityTaskWoken);
}

BaseType_t Timer::stop_from_isr(BaseType_t &pxHigherPriorityTaskWoken)
{
    return xTimerStopFromISR(timer, &pxHigherPriorityTaskWoken);
}

BaseType_t Timer::change_period_from_isr(TickType_t xNewPeriod,
                                         BaseType_t &pxHigherPriorityTaskWoken)
{
    return xTimerChangePeriodFromISR(
        timer, xNewPeriod, &pxHigherPriorityTaskWoken);
}

BaseType_t Timer::reset_from_isr(BaseType_t &pxHigherPriorityTaskWoken)
{
    return xTimerResetFromISR(timer, &pxHigherPriorityTaskWoken);
}

void Timer::set_reload_mode(UBaseType_t uxAutoReload)
{
    vTimerSetReloadMode(timer, uxAutoReload);
}

const char *Timer::get_name() const { return pcTimerGetName(timer); }

TickType_t Timer::get_period() const { return xTimerGetPeriod(timer); }

TickType_t Timer::get_expiry_time() const { return xTimerGetExpiryTime(timer); }

BaseType_t Timer::get_reload_mode() const
{
    return static_cast<BaseType_t>(uxTimerGetReloadMode(timer));
}

} // namespace shrapnel::os
