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

/** \file
 * Mapping MIDI commands to audio parameter changes.
 *
 * \page ws_api websockets JSON API
 *
 * This page documents the JSON-based message passing API used to communicate
 * between the Flutter frontend and the embedded firmware. The API works by
 * passing messages across a websockets connection. This allows the firmware to
 * send events to the frontend without polling. It is useful e.g. when an audio
 * parameter is updated through MIDI, and the new value should be displayed in
 * the UI.
 *
 * All messages have a string valued member `messageType`, used to identify
 * what operation should be performed.
 *
 * Messages may receive no response, a single response message or multiple
 * responses messages. This is documented separately for each message.
 *
 * A message with `messageType` set to `hello` and a single parameter
 * `example_parameter` with an integer value 1 looks like this:
 *
 * ~~~
 * {
 *   "messageType": "hello",
 *   "example_parameter": 1,
 * }
 * ~~~
 *
 * See also:
 * - \subpage ws_api_parameters
 * - \subpage ws_api_midi
 *
 * \page ws_api_midi MIDI mapping JSON messages
 *
 * - Get list of all mappings
 * - Create a new mapping
 * - Update a mapping
 * - Delete a mapping
 *
 * \todo how to nicely show a JSON object template/schema? AWS docs include a
 * few of those, maybe reference their style.
 *
 * ~~~
 * MidiMapping:
 *   midi_channel: int
 *   cc_number: int
 *   parameter_id: string
 * ~~~
 *
 * \note In the examples of this table, UUIDs are replaced with `...` for
 * brevity. Normally these would look like
 * `12345678-abcd-0000-0000-000000000000`.
 *
 * <table>
 * <tr><th> Message type <th> Parameters <th> Direction <th> Behaviour <th> Example
 * <tr>
 *   <td> `MidiMap::get::request`
 *   <td> None
 *   <td> UI -> Firmware
 *   <td> The firmware will send a single `MidiMap::get::response` that
 *        includes a list of all MIDI mappings.
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::get::request"
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::get::response`
 *   <td> `mappings` (map of `UUID` to `MidiMapping`): The list of all MIDI mappings
 *   <td> Firmware -> UI
 *   <td> Sent in response to `MidiMap::get::request`
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::get::response",
 *     "mappings": {
 *       "...": { "midi_channel": 1, "cc_number": 2, "parameter_id": "gain" },
 *       "...": { "midi_channel": 3, "cc_number": 4, "parameter_id": "tone" }
 *     }
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::create::request`
 *   <td> `mapping` (`MidiMapping`): The MIDI mapping to create
 *   <td> UI -> Firmware
 *   <td> Firmware creates a new MIDI mapping according to the parameters.
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::create::request",
 *     "mapping": {
 *       "...": {
 *         "midi_channel": 1,
 *         "cc_number": 1,
 *         "parameter_id": "gain"
 *       }
 *     }
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::create::response`
 *   <td> `mapping` (`MidiMapping`): The newly created MIDI mapping
 *   <td> Firmware -> UI
 *   <td> Indicates that a new MIDI mapping was created successfully.
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::create::response",
 *     "mapping": {
 *       "...": {
 *         "midi_channel": 1,
 *         "cc_number": 1,
 *         "parameter_id": "gain"
 *       }
 *     }
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::update`
 *   <td> `mapping` (`MidiMapping`): The MIDI mapping to update
 *   <td> UI -> Firmware
 *   <td> Firmware updates an existing MIDI mapping according to the parameters.
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::update",
 *     "mapping": {
 *       "...": {
 *         "midi_channel": 1,
 *         "cc_number": 1,
 *         "parameter_id": "gain"
 *       }
 *     }
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::remove`
 *   <td> `id` (`UUID`): The ID of the MIDI mapping to remove
 *   <td> UI -> Firmware
 *   <td> Firmware removes the specified MIDI mapping .
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::remove",
 *     "id": "..."
 *   }
 *   ~~~
 * </table>
 *
 */
#pragma once

#include <array>
#include <cstdint>
#include <esp_log.h>

#include "audio_param.h"
#include "midi.h"

namespace shrapnel {
namespace midi {

struct Mapping {
    using id_t = std::array<uint8_t, 16>;

    uint8_t midi_channel;
    uint8_t cc_number;
    parameters::id_t parameter_name;

    Mapping(
            uint8_t a_midi_channel,
            uint8_t a_cc_number,
            parameters::id_t a_parameter_name
    ) : midi_channel{a_midi_channel},
        cc_number{a_cc_number},
        parameter_name{a_parameter_name} {};

    std::strong_ordering operator<=>(const Mapping &other) const = default;
};

template<typename AudioParametersT, std::size_t N>
class MappingManager final {
    public:
    using MapType = etl::map<Mapping::id_t, Mapping, N>;

    MappingManager(std::shared_ptr<AudioParametersT> a_parameters) : parameters{a_parameters} {};

    MapType get() {
        return mappings;
    };
    /// \return non-zero on failure
    int create(const std::pair<const Mapping::id_t, Mapping> &mapping) {
        if(mappings.full()) {
            ESP_LOGE(TAG, "Failed to create new midi mapping, map is full");
            return -1;
        }

        mappings.insert(mapping);
        return 0;
    };
    /// \return non-zero on failure
    int update(const std::pair<const Mapping::id_t, Mapping> &mapping) {
        if(!mappings.contains(mapping.first))
        {
            ESP_LOGE(TAG, "Does not contain key");
            ESP_LOG_BUFFER_HEX_LEVEL(TAG, mapping.first.data(), mapping.first.size(), ESP_LOG_ERROR);
            return -1;
        }

        mappings.erase(mapping.first);
        mappings.insert(mapping);
        return 0;
    };
    void remove(const Mapping::id_t &id) {
        mappings.erase(id);
    };

    /** React to a MIDI message by updating an audio parameter if there is a
     * mapping registered
     */
    void process_message(Message message) {
        auto cc_params = get_if<Message::ControlChange>(&message.parameters);
        if(!cc_params) return;

        for(const auto &mapping : mappings)
        {
            if(mapping.second.midi_channel != message.channel)
            {
                continue;
            }

            if(mapping.second.cc_number != cc_params->control)
            {
                continue;
            }

            parameters->update(
                    mapping.second.parameter_name,
                    cc_params->value / float(CC_VALUE_MAX));
        }
    };

    private:
    std::shared_ptr<AudioParametersT> parameters;
    MapType mappings;

    static constexpr char TAG[] = "midi_mapping";
};

}
}
