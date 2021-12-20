#include "cmd_handling_task.h"

namespace shrapnel {

CommandHandlingTask::CommandHandlingTask(int priority,
        QueueBase<cmd_message_t> *q,
        AudioParametersBase *param) :
    TaskBase("command handling", 4000, priority)
{
    cmd_init(q, param);
    start();
}

void CommandHandlingTask::loop(void)
{
    cmd_task_work(nullptr);
}

}
