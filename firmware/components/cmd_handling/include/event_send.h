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
 *
 */

#pragma once

#include "audio_events.h"

namespace shrapnel {

class EventSendBase {
    public:
    virtual void send(const char *json, int fd) = 0;
    virtual void send(const char *json) = 0;
};

class EventSend final : public EventSendBase {
    public:
    void send(const char *json, int fd) override
    {
        audio_event_send_callback(json, fd);
    }

    void send(const char *json) override
    {
        send(json, -1);
    }
};

}
