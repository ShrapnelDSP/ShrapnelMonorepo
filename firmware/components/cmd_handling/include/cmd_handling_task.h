#pragma once

#include "cmd_handling.h"
#include "task.h"

namespace shrapnel {

class CommandHandlingTask : public TaskBase
{
    public:
    CommandHandlingTask(int priority,
                        QueueBase<CommandHandling::Message> *queue,
                        AudioParametersBase *param);

    CommandHandling cmd;
    private:
        void loop(void) override;
};

}
