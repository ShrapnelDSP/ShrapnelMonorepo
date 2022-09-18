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
#include <etl/delegate.h>

namespace shrapnel {
namespace midi {

Decoder::Decoder(Callback _on_message_decoded) :
    on_message_decoded{_on_message_decoded},
    state{IDLE},
    current_status{0},
    data_count{0} {}

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
    /* Running status allows data to be sent with no status byte. In this case,
     * the previous status is reused. */
    if(!is_status_byte(byte) && is_status_byte(current_status))
    {
        auto next_state = decode_message(byte);
        return next_state;
    }

    switch(byte & 0xF0)
    {
    case CONTROL_CHANGE:
    case NOTE_ON:
    case NOTE_OFF:
    case PROGRAM_CHANGE:
        current_status = byte;
        return GOT_MESSAGE;
    }
    return IDLE;
}

Decoder::State Decoder::decode_message(uint8_t byte)
{
    assert(is_status_byte(current_status));

    switch(current_status & 0xF0)
    {
    case CONTROL_CHANGE:
    case NOTE_ON:
    case NOTE_OFF:
        received_data[data_count] = byte;
        data_count++;
        if(data_count == 2)
        {
            output_message();
            data_count = 0;
            return IDLE;
        }
        break;
    case PROGRAM_CHANGE:
        received_data[0] = byte;
        output_message();
        return IDLE;
    default:
        assert(false);
        break;
    }

    return GOT_MESSAGE;
}

void Decoder::output_message()
{
    Message message{};

    switch(current_status & 0xF0)
    {
    case NOTE_ON:
        // TODO how to use a designated initialiser here? This is pretty hard
        //      to read.
        message.parameters.emplace<Message::NOTE_ON>(
            received_data[0],
            received_data[1]
        );
        break;
    case NOTE_OFF:
        message.parameters.emplace<Message::NOTE_OFF>(
            received_data[0],
            received_data[1]
        );
        break;
    case CONTROL_CHANGE:
        // Channel Mode messages should be ignored
        if((received_data[0] & 0xF8) == 0x78)
        {
            return;
        }

        message.parameters.emplace<Message::CONTROL_CHANGE>(
            received_data[0],
            received_data[1]
        );
        break;
    case PROGRAM_CHANGE:
        message.parameters.emplace<Message::PROGRAM_CHANGE>(received_data[0]);
        break;
    default:
        assert(false);
        break;
    }

    message.channel = (current_status & 0x0F) + 1;
    on_message_decoded(message);
}

}
}
