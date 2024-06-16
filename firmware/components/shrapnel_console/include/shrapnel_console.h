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

extern "C" {
#include "embedded_cli.h"
}

#include "midi_protocol.h"
#include <cstddef>
#include <etl/delegate.h>

namespace shrapnel {

using MidiMessageSendCallback = etl::delegate<void(const midi::Message &)>;
using WifiSetupCallback =
    etl::delegate<void(const char *ssid, const char *password)>;

class Console
{
public:
    Console(MidiMessageSendCallback a_midi_message_callback,
            WifiSetupCallback a_wifi_setup_callback);

    void handle_character(char c);

private:
    friend int handle_midi(int argc, char *argv[]);
    friend int handle_wifi(int argc, char *argv[]);

    static void putch(void *, char c, bool);

    MidiMessageSendCallback midi_message_callback;
    WifiSetupCallback wifi_setup_callback;

    struct embedded_cli cli;
};
} // namespace shrapnel
