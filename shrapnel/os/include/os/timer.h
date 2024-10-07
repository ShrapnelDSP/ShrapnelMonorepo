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
#pragma once

#include "etl/delegate.h"
#include <memory>
#include <optional>

namespace shrapnel::os {

enum class timer_error
{
    TIMER_START_SUCCESS,
    TIMER_START_FAILURE,
};

class Timer final
{
public:
    using Callback = etl::delegate<void(void)>;

    Timer(const char *pcTimerName,
          uint32_t xTimerPeriod,
          bool uxAutoReload,
          std::optional<Callback> callback = std::nullopt);

    ~Timer();

    [[nodiscard]] bool is_active() const;

    [[nodiscard]] timer_error start(uint32_t xBlockTime);
    [[nodiscard]] timer_error stop(uint32_t xBlockTime);

    struct impl;

private:
    // PImpl pattern used to abstract away the platform differences between
    // FreeRTOS used for embedded code and fake timers used for testing
    std::unique_ptr<impl> p_impl;
};

} // namespace shrapnel::os
