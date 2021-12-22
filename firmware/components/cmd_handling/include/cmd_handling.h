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

    struct ParamEntry
    {
        /** The string identifier of the parameter */
        const char *name;
        /** The binary representation of the parameter */
        audio_param_t id;
    };

    /** \brief
     *
     * \param[in] queue Used to receive JSON messages containing parameter updates.
     * \param[in] param Data received through \ref queue are
     * translated to binary and sent to this object.
     */
    CommandHandling(QueueBase<Message> *queue, AudioParametersBase *param);

    void register_parameter(ParamEntry entry);

    void work(void);

    private:

    audio_param_t get_id_for_param(const char *name);

    QueueBase<Message> *queue;
    AudioParametersBase *param;
    /* TODO how to avoid having to increase size manually when
     * more effects are added?
     *
     * We don't want to use heap allocation when the parameters
     * are registered. Maybe some constexpr functions could be
     * used to register the parameters?
     */
    etl::list<ParamEntry, 8> param_table;
};

}
