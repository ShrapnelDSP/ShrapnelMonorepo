#include "cmd_handling_task.h"

namespace shrapnel {

CommandHandlingTask::CommandHandlingTask(int priority,
        QueueBase<CommandHandling::Message> *queue,
        AudioParametersBase *param) :
    TaskBase("command handling", 4000, priority),
    cmd(queue, param)
{
    start();
}

void CommandHandlingTask::loop(void)
{
    cmd.work();
}

}
