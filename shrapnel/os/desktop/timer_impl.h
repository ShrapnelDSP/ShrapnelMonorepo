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

#include "etl/callback_timer_atomic.h"
#include "os/timer.h"

namespace shrapnel::os {

struct Timer::impl
{
    impl(const char *pcTimerName,
         uint32_t xTimerPeriod,
         bool uxAutoReload,
         std::optional<Callback> callback);

    [[nodiscard]] bool is_active() const;
    timer_error start(uint32_t xBlockTime);
    timer_error stop(uint32_t xBlockTime);

    // Advance tick count, firing callbacks for any expired timers

    // TODO how to make this globally accessible? Should we dependency inject
    // something that has a tick method? It seems quite tedious to pass this
    // down to the lowest level code using timers. This is rougly the same
    // problem solved by provider in flutter. Might just have to use a global
    // variable for this.
    static void tick(uint32_t tick_count);

private:
    void do_callback()
    {
        if(callback.has_value())
            (*callback)();
        is_running = false;
    }

    etl::timer::id::type id;
    bool is_running = false;
    std::optional<Callback> callback;
    static etl::callback_timer_atomic<254> timers;
};

} // namespace shrapnel::os