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

#include <memory>

#include "freertos/portmacro.h"
#include "os/timer.h"
#include "timer_impl.h"

namespace shrapnel::os {

extern "C" void timer_callback(TimerHandle_t a_timer)
{
    auto timer = reinterpret_cast<shrapnel::os::Timer::impl *>(
        pvTimerGetTimerID(a_timer));
    (*timer->callback)();
}

Timer::impl::impl(const char *pcTimerName,
                  TickType_t xTimerPeriod,
                  const UBaseType_t uxAutoReload,
                  std::optional<etl::delegate<void(void)>> callback)
{
    timer = xTimerCreate(pcTimerName,
                         xTimerPeriod,
                         uxAutoReload,
                         this,
                         callback.has_value() ? timer_callback : nullptr);
}

Timer::impl::~impl()
{
    auto rc = xTimerDelete(timer, portMAX_DELAY);
    assert(rc == pdPASS);
}

BaseType_t Timer::impl::is_active() const { return xTimerIsTimerActive(timer); }

BaseType_t Timer::impl::start(TickType_t xBlockTime)
{
    return xTimerStart(timer, xBlockTime);
}

BaseType_t Timer::impl::stop(TickType_t xBlockTime)
{
    return xTimerStop(timer, xBlockTime);
}

} // namespace shrapnel::os
