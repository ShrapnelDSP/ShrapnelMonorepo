#pragma once

#include "cmd_handling.h"
#include "task.h"

namespace shrapnel {

class CommandHandlingTask : public TaskBase
{
    public:
    CommandHandlingTask(int priority,
                        QueueBase<cmd_message_t> *q,
                        AudioParametersBase *param);

    private:
        void loop(void) override;
};

}
