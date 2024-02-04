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

Timer::~Timer() {}

BaseType_t Timer::is_active() const { return p_impl->is_active(); }

BaseType_t Timer::start(TickType_t xBlockTime)
{
    return p_impl->start(xBlockTime);
}

BaseType_t Timer::stop(TickType_t xBlockTime)
{
    return p_impl->stop(xBlockTime);
}

} // namespace shrapnel::os