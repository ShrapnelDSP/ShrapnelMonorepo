#pragma once

#include "queue.h"
#include "audio_param.h"

namespace shrapnel {

class CommandHandling
{
    public:
    struct Message
    {
        char json[128];
    };

    CommandHandling(QueueBase<Message> *queue, AudioParametersBase *param);

    void work(void);

    private:
    QueueBase<Message> *queue;
    AudioParametersBase *param;
};

}
