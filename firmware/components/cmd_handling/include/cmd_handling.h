#pragma once

#include "queue.h"
#include "audio_param.h"
#include "etl/list.h"

namespace shrapnel {

class CommandHandling
{
    public:
    struct Message
    {
        char json[128];
    };

    /** \brief
     *
     * \param[in] queue Used to receive JSON messages containing parameter updates.
     * \param[in] param Data received through \ref queue are
     * translated to binary and sent to this object.
     */
    CommandHandling(QueueBase<Message> *queue, AudioParametersBase *param);

    void work(void);

    private:

    QueueBase<Message> *queue;
    AudioParametersBase *param;
};

}
