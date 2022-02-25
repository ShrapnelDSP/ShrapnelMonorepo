#pragma once

#include "audio_events.h"

namespace shrapnel {

class EventSendBase {
    public:
    virtual void send(char *json, int fd) = 0;
};

class EventSend : public EventSendBase {
    public:
    void send(char *json, int fd) override
    {
        audio_event_send_callback(json, fd);
    }
};

}
