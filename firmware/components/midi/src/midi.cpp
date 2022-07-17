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

#include "midi.h"
#include "esp_log.h"

namespace shrapnel {
namespace midi {

Decoder::Decoder(std::function<void(Message)> _on_message_decoded) : on_message_decoded{_on_message_decoded}, state{IDLE}, current_message{0}, data_count{0} {
}

void Decoder::decode(uint8_t byte)
{
    State new_state = IDLE;
    switch(state)
    {
    case IDLE:
        new_state = decode_idle(byte);
        break;
    case GOT_MESSAGE:
        new_state = decode_message(byte);
        break;
    default:
        ESP_LOGE(TAG, "Unhandled decoder state %d", state);
        break;
    }

   state = new_state;
}

Decoder::State Decoder::decode_idle(uint8_t byte)
{
    if(byte == NOTE_ON)
    {
        current_message = byte;
        return GOT_MESSAGE;
    }
    return IDLE;
}

Decoder::State Decoder::decode_message(uint8_t byte)
{
    assert(is_status_byte(current_message));

    switch(current_message)
    {
    case NOTE_ON:
        received_data[data_count] = byte;
        data_count++;
        if(data_count == 2)
        {
            on_message_decoded({
                .type{NOTE_ON},
                .note_on{
                    .note{received_data[0]},
                    .velocity{received_data[1]}
                },
            });
            current_message = 0;
            data_count = 0;
            return IDLE;
        }
        break;
    default:
        assert(false);
        break;
    }

    return GOT_MESSAGE;
}

}
}
