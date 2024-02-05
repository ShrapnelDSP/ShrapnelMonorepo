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

#include "timer_impl.h"
#include "os/timer.h"

namespace shrapnel::os {

Timer::impl::impl(const char *pcTimerName,
                  uint32_t xTimerPeriod,
                  bool uxAutoReload,
                  std::optional<etl::delegate<void(void)>> callback)
    : callback(callback)
{
    timers.enable(true);
    id = timers.register_timer(
        Callback::create<Timer::impl, &Timer::impl::do_callback>(*this),
        xTimerPeriod,
        uxAutoReload);
}

bool Timer::impl::is_active() const { return is_running; }

timer_error Timer::impl::start(uint32_t xBlockTime)
{
    is_running = true;
    return timers.start(id) ? timer_error::TIMER_START_SUCCESS
                            : timer_error::TIMER_START_FAILURE;
}

timer_error Timer::impl::stop(uint32_t xBlockTime)
{
    is_running = false;
    return timers.stop(id) ? timer_error::TIMER_START_SUCCESS
                           : timer_error::TIMER_START_FAILURE;
}

void Timer::impl::tick(uint32_t tick_count) { timers.tick(tick_count); }

etl::callback_timer_atomic<254> Timer::impl::timers;

} // namespace shrapnel::os
