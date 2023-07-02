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

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "os/timer.h"

namespace shrapnel::os {

extern "C" void timer_callback(TimerHandle_t a_timer);

struct Timer::impl
{
    impl(const char *pcTimerName,
         TickType_t xTimerPeriod,
         UBaseType_t uxAutoReload,
         std::optional<etl::delegate<void(void)>> callback);

    ~impl();

    [[nodiscard]] BaseType_t is_active() const;
    BaseType_t start(TickType_t xBlockTime);
    BaseType_t stop(TickType_t xBlockTime);

    friend void shrapnel::os::timer_callback(TimerHandle_t a_timer);
    TimerHandle_t timer;
    std::optional<Callback> callback;
};

} // namespace shrapnel::os