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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace shrapnel {

class TaskBase
{
    public:
    TaskBase(const char *name, size_t stack, int priority);
    ~TaskBase(void);

    protected:
    /** Called once when the task starts. */
    virtual void setup(void);

    /** Called continously while the task is running.
     *
     * \note The task should block in this function to avoid task starvation. */
    virtual void loop(void) = 0;

    /** Call this from the derived class' constructor as the last step to start
     * the task.
     *
     * Prevents the task running before the constructor completes.
     */
    void start(void);

    private:
    static void task_thread(void *param);

    const char *name;
    size_t stack;
    int priority;
    TaskHandle_t handle;
};

};
