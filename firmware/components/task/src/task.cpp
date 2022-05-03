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

#include "task.h"

#include <cassert>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace shrapnel {

TaskBase::TaskBase(const char *a_name, size_t a_stack, unsigned int a_priority) :
    name(a_name),
    stack(a_stack),
    priority(a_priority) {}

void TaskBase::task_thread(void *param)
{
    TaskBase *task = static_cast<TaskBase *>(param);

    task->setup();
    while (1)
    {
        task->loop();
    }
}

void TaskBase::setup(void)
{
}

void TaskBase::start(void)
{
    int rc = xTaskCreate(task_thread, name, stack, this, priority, &handle);
    assert(rc == pdPASS);
}

TaskBase::~TaskBase(void)
{
    vTaskDelete(handle);
}

}
