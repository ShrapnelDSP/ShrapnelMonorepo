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

#include "cmd_handling.h"
#include "task.h"

namespace shrapnel {

template<typename AudioParametersT>
class CommandHandlingTask : public TaskBase
{
    public:
    CommandHandlingTask(int priority,
                        QueueBase<typename CommandHandling<AudioParametersT>::Message> *queue,
                        AudioParametersT *param) :
        TaskBase("command handling", 4000, priority),
        cmd(queue, param)
    {
        start();
    }

    CommandHandling<AudioParametersT> cmd;

    private:
    void loop(void) override
    {
        cmd.work();
    }
};

}
