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
#include "freertos/portmacro.h"
#include "freertos/timers.h"
#include "etl/delegate.h"

namespace shrapnel::os {

// TODO is there a way to hide this from the public interface?
extern "C" void timer_callback(TimerHandle_t a_timer);

class Timer final
{
public:
    Timer(const char *pcTimerName,
          TickType_t xTimerPeriod,
          const UBaseType_t uxAutoReload,
          etl::delegate<void(void)> callback);

    ~Timer();

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    BaseType_t is_active() const;

    BaseType_t start(TickType_t xBlockTime);
    BaseType_t stop(TickType_t xBlockTime);
    BaseType_t change_period(TickType_t xNewPeriod, TickType_t xBlockTime);
    BaseType_t reset(TickType_t xBlockTime);

    BaseType_t start_from_isr(BaseType_t &pxHigherPriorityTaskWoken);
    BaseType_t stop_from_isr(BaseType_t &pxHigherPriorityTaskWoken);
    BaseType_t change_period_from_isr(TickType_t xNewPeriod, BaseType_t &pxHigherPriorityTaskWoken);
    BaseType_t reset_from_isr(BaseType_t &pxHigherPriorityTaskWoken);

    void set_reload_mode(UBaseType_t uxAutoReload);

    const char *get_name() const;

    TickType_t get_period() const;
    TickType_t get_expiry_time() const;
    BaseType_t get_reload_mode() const;

private:
    friend void shrapnel::os::timer_callback(TimerHandle_t);
    TimerHandle_t timer;
    etl::delegate<void(void)> callback;
};

}
