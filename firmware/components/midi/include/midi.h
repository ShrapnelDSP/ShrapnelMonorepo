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

#include <assert.h>
#include <cstdint>
#include <etl/string_stream.h>

namespace shrapnel {
namespace midi {

struct Message;

enum MessageType {
    NOTE_OFF = 0x80,
    NOTE_ON = 0x90,
    CONTROL_CHANGE = 0xB0,
    PROGRAM_CHANGE = 0xC0,
};

struct Message {
    struct NoteOnOff {
        uint8_t note;
        uint8_t velocity;
    };

    struct ControlChange {
        uint8_t control;
        uint8_t value;
    };

    struct ProgramChange {
        uint8_t number;
    };

    MessageType type;
    union {
        NoteOnOff note_on;
        NoteOnOff note_off;
        ControlChange control_change;
        ProgramChange program_change;
    };

    friend etl::string_stream& operator<<(etl::string_stream&  out, const Message& message) {
        out << "{";

        switch(message.type)
        {
        case NOTE_ON:
            out << "note on " << +message.note_on.note << " " << +message.note_on.velocity;
            break;
        case NOTE_OFF:
            out << "note off " << +message.note_on.note << " " << +message.note_on.velocity;
            break;
        case CONTROL_CHANGE:
            out << "control change " << +message.control_change.control << " " << +message.control_change.value;
            break;
        case PROGRAM_CHANGE:
            out << "program change " << +message.program_change.number;
            break;
        default:
            out << +message.type;
            break;
        }

        out << "}";
        return out;
    }

#if defined(TESTING)
#endif
};

class Decoder {
    public:
    using Callback = std::function<void(Message)>;

    /**
     * \param on_message_decoded Called when a message has been decoded
     */
    Decoder(Callback on_message_decoded);

    /** Decode bytes received by UART
     *
     * Call this function every time a byte has been received by UART
     */
    void decode(uint8_t byte);

    private:

    void output_message();


    Callback on_message_decoded;

    static inline bool is_status_byte(uint8_t byte) {
        return byte & (1 << 7);
    };

    enum State {
        IDLE,
        GOT_MESSAGE,
    };

    State state;

    State decode_idle(uint8_t byte);
    State decode_message(uint8_t byte);

    uint8_t current_status;
    std::size_t data_count;
    std::array<uint8_t, 2> received_data;

    constexpr static const char *TAG = "midi";
};

/** Implements persistent configuration for MIDI functionality
 *
 * This is used to store the basic channel number and other configuration that
 * should be persistent after power down.
 *
 * \tparam T An implementation of \ref PersistentStorage
 */
template<typename T>
class Configuration {
    int getBasicChannel();
    void setBasicChannel(int channel);
};

/** Interface for storing persistent data
 *
 * The data sent to \ref save should be reloaded by \ref load even after power
 * down.
 */
class PeristentStorage {
    virtual void save(const char *key, const void *data, std::size_t data_size) = 0;
    virtual int load(const char *key, void *data, std::size_t data_size) = 0;
};

}
}
