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

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace shrapnel {
namespace midi {

enum MessageType {
    CHANNEL_VOICE,
    CHANNEL_MODE,
    SYSTEM_COMMON,
    SYSTEM_REAL_TIME,
    SYSTEM_EXCLUSIVE,
};

enum ChannelVoiceMessageType {
    NOTE_OFF = 0x80,
    NOTE_ON = 0x90,
    POLY_KEY_PRESSURE = 0xA0,
    CONTROL_CHANGE = 0xB0,
    PROGRAM_CHANGE = 0xC0,
    CHANNEL_PRESSURE = 0xD0,
    PITCH_BEND = 0xE0,
};

struct Message {
    MessageType type;
    union Data {
        struct Voice {
            ChannelVoiceMessageType type;
            union ChannelVoiceData {
                struct {
                    uint8_t note;
                    uint8_t velocity;
                } note_on;
                struct {
                    uint8_t note;
                    uint8_t velocity;
                } note_off;
            };
        } voice;
    };
};

class Decoder {
    public:
    /**
     * \param on_message_decoded Called when a message has been decoded
     */
    Decoder(std::function<void(Message)> on_message_decoded);

    /** Decode bytes received by UART
     *
     * Call this function every time a byte has been received by UART
     */
    void decode(uint8_t byte);

    private:
    static inline bool is_status_byte(uint8_t byte) {
        return byte & (1 << 7);
    };

    enum State {
        IDLE,
    };

    State state;

    State decode_idle(uint8_t byte);

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
