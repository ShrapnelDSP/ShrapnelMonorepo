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
#include <compare>
#include <cstdint>
#include <etl/delegate.h>
#include <etl/string_stream.h>
#include <variant>

namespace shrapnel {
namespace midi {

enum MessageType {
    NOTE_OFF = 0x80,
    NOTE_ON = 0x90,
    CONTROL_CHANGE = 0xB0,
    PROGRAM_CHANGE = 0xC0,
};

struct Message {
    using midi_channel_t = uint8_t ;

    struct NoteOn {
        uint8_t note;
        uint8_t velocity;

        std::strong_ordering operator<=>(const NoteOn &other) const = default;
    };

    struct NoteOff {
        uint8_t note;
        uint8_t velocity;

        std::strong_ordering operator<=>(const NoteOff &other) const = default;
    };

    struct ControlChange {
        uint8_t control;
        uint8_t value;

        std::strong_ordering operator<=>(const ControlChange &other) const = default;
    };

    struct ProgramChange {
        uint8_t number;

        std::strong_ordering operator<=>(const ProgramChange &other) const = default;
    };

    midi_channel_t channel;
    std::variant<NoteOn, NoteOff, ControlChange, ProgramChange> parameters;

    std::weak_ordering operator<=>(const Message &other) const = default;

    friend etl::string_stream& operator<<(etl::string_stream&  out, const Message& message) {
        out << "{ channel " << message.channel << " ";

        auto print_parameters = [&](auto &param) {
            using T = std::decay_t<decltype(param)>;

            if constexpr (std::is_same_v<T, NoteOn>) {
                out << "note on " << +param.note << " " << +param.velocity;
            } else if constexpr (std::is_same_v<T, NoteOff>) {
                out << "note off " << +param.note << " " << +param.velocity;
            } else if constexpr (std::is_same_v<T, ControlChange>) {
                out << "control change " << +param.control << " " << +param.value;
            } else if constexpr (std::is_same_v<T, ProgramChange>) {
                out << "program change " << +param.number;
            } else {
                // TODO is there any way to turn this into a compiler error?
                out << "unknown";
            }
        };

        std::visit(print_parameters, message.parameters);

        out << " }";
        return out;
    }
};

class Decoder {
    public:
    using Callback = etl::delegate<void(Message)>;

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
    public:
    int getBasicChannel();
    void setBasicChannel(int channel);
};

/** Interface for storing persistent data
 *
 * The data sent to \ref save should be reloaded by \ref load even after power
 * down.
 */
class PersistentStorage {
    public:
    virtual void save(const char *key, const void *data, std::size_t data_size) = 0;
    virtual int load(const char *key, void *data, std::size_t data_size) = 0;
};

}
}
