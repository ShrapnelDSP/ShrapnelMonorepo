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
 * \note The messages should be designed to be compatible with the Freezed
 *       Dart package. https://pub.dev/packages/freezed#fromjsontojson
 *
 * \note This can be made easier if unions or sum types use the `runtimeType`
 *       field as the type tag. Some messages don't do this, but it can be
 *       worked around using annotations provided by Freezed.
 *
 * See also:
 * - \subpage ws_api_events
 * - \subpage ws_api_midi
 * - \subpage ws_api_parameters
 *
 * \page ws_api_midi MIDI mapping JSON messages
 *
 * This page documents the message format used for messages related to managing
 * the mapping of MIDI messages to audio parameters.
 *
 * The `MidiMapping` object is defined as follows:
 *
 * ~~~
 * {
 *   "midi_channel": integer,
 *   "cc_number": integer,
 *   "mode": "parameter" or "toggle",
 *   "parameter_id": "string",
 * }
 * ~~~
 *
 * The `MidiMessage` object has four possible formats:
 *
 * ~~~
 * {
 *   "runtimeType": "noteOn",
 *   "channel": integer,
 *   "note": integer,
 *   "velocity": integer,
 * }
 * ~~~
 *
 * ~~~
 * {
 *   "runtimeType": "noteOff",
 *   "channel": integer,
 *   "note": integer,
 *   "velocity": integer,
 * }
 * ~~~
 *
 * ~~~
 * {
 *   "runtimeType": "controlChange",
 *   "channel": integer,
 *   "control": integer,
 *   "value": integer,
 * }
 * ~~~
 *
 * ~~~
 * {
 *   "runtimeType": "programChange"
 *   "channel": integer,
 *   "number": integer
 * }
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
 *       "...": {
 *         "midi_channel": 1,
 *         "cc_number": 2,
 *         "mode": "parameter",
 *         "parameter_id": "gain"
 *       },
 *       "...": {
 *         "midi_channel": 3,
 *         "cc_number": 4,
 *         "mode": "toggle",
 *         "parameter_id": "tone"
 *       }
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
 *         "mode": "parameter",
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
 *         "mode": "parameter",
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
 *         "mode": "parameter",
 *         "parameter_id": "gain"
 *       }
 *     }
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::destroy`
 *   <td> `id` (`UUID`): The ID of the MIDI mapping to destroy
 *   <td> UI -> Firmware
 *   <td> Firmware removes the specified MIDI mapping .
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::destroy",
 *     "id": "..."
 *   }
 *   ~~~
 * <tr>
 *   <td> `MidiMap::midi_message_received`
 *   <td> `message` (`MidiMessage`): The MIDI message
 *   <td> Firmware -> UI
 *   <td> Firmware notifies this periodically, so that the frontend can react
 *        to MIDI messages, for example to connect a parameter to the MIDI
 *        message.
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "MidiMap::midi_message_received",
 *     "message": {
 *       "runtimeType": "controlChange",
 *       "channel": 1,
 *       "control": 2,
 *       "value": 3
 *     }
 *   }
 *   ~~~
 * </table>
 *
 */
#pragma once

#include <array>
#include <cinttypes>
#include <cstdint>
#include <esp_log.h>
#include <span>

#include "audio_param.h"
#include "crud.h"
#include "midi_mapping_api.h"
#include "midi_protocol.h"

namespace shrapnel::midi {

using MappingObserver = etl::observer<const Mapping::id_t &>;

template <typename AudioParametersT,
          std::size_t MAX_MAPPINGS,
          std::size_t MAX_OBSERVERS>
class MappingManager final
    : public etl::observable<MappingObserver, MAX_OBSERVERS>,
      public persistence::Crud<Mapping>
{
public:
    using MapType = etl::map<Mapping::id_t, Mapping, MAX_MAPPINGS>;

    explicit MappingManager(
        std::shared_ptr<AudioParametersT> a_parameters,
        std::unique_ptr<persistence::Crud<std::span<uint8_t>>> a_storage)
        : parameters{a_parameters},
          storage{std::move(a_storage)}
    {
        for_each(
            [this](uint32_t id, const shrapnel::midi::Mapping &mapping) {
                mappings.insert({id, mapping});
            });
    }

    [[nodiscard]] const etl::imap<Mapping::id_t, Mapping> *get() const
    {
        return &mappings;
    }

    /// \return non-zero on failure
    [[nodiscard]] int create(const Mapping &mapping,
                             Mapping::id_t &id_out) override
    {
        if(mappings.full())
        {
            ESP_LOGE(TAG, "Failed to create new midi mapping, map is full");
            return -1;
        }

        std::array<uint8_t, 256> memory{};
        std::span<uint8_t> buffer{memory};

        auto encoded = api::to_bytes(mapping, buffer);
        if(!encoded.has_value())
        {
            return -1;
        }

        int rc = storage->create(*encoded, id_out);
        if(rc != 0)
        {
            return -1;
        }

        mappings.insert({id_out, mapping});
        this->notify_observers(id_out);
        return 0;
    }

    [[nodiscard]] int read(Mapping::id_t id, Mapping &mapping) override
    {
        std::array<uint8_t, 256> memory{};
        std::span<uint8_t> buffer{memory};

        int rc = storage->read(id, buffer);
        if(rc != 0)
        {
            return -1;
        }

        auto decoded = api::from_bytes<Mapping>(buffer);
        if(!decoded.has_value())
        {
            return -1;
        }

        mappings.insert({id, *decoded});
        mapping = *decoded;
        this->notify_observers(id);
        return 0;
    }

    /// \return non-zero on failure
    [[nodiscard]] int update(Mapping::id_t id, const Mapping &mapping) override
    {
        if(!mappings.contains(id))
        {
            ESP_LOGE(TAG, "Does not contain key %" PRIu32, id);
            return -1;
        }

        std::array<uint8_t, 256> memory{};
        std::span<uint8_t> buffer{memory};

        auto encoded = api::to_bytes(mapping, buffer);
        if(!encoded.has_value())
        {
            return -1;
        }

        int rc = storage->update(id, *encoded);
        if(rc != 0)
        {
            return -1;
        }

        mappings.erase(id);
        mappings.insert({id, mapping});
        this->notify_observers(id);
        return 0;
    }

    [[nodiscard]] int destroy(Mapping::id_t id) override
    {
        int rc = storage->destroy(id);
        if(rc != 0)
        {
            return -1;
        }

        mappings.erase(id);
        this->notify_observers(id);
        return 0;
    }

    void
    for_each(etl::delegate<void(uint32_t, const Mapping &)> callback) override
    {
        storage->for_each(
            [&callback](uint32_t id, const std::span<uint8_t> &buffer)
            {
                auto decoded = api::from_bytes<Mapping>(buffer);
                if(!decoded.has_value())
                {
                    return;
                }

                callback(id, *decoded);
            });
    }

    /** React to a MIDI message by updating an audio parameter if there is a
     * mapping registered
     */
    void process_message(Message message) const
    {
        auto cc_params = get_if<Message::ControlChange>(&message.parameters);
        if(!cc_params)
            return;

        for(const auto &[_, mapping] : mappings)
        {
            if(mapping.midi_channel != message.channel)
            {
                continue;
            }

            if(mapping.cc_number != cc_params->control)
            {
                continue;
            }

            switch(mapping.mode)
            {
            case Mapping::Mode::PARAMETER:
                parameters->update(mapping.parameter_name,
                                   cc_params->value / float(CC_VALUE_MAX));
                break;
            case Mapping::Mode::TOGGLE:
                if(cc_params->value == 0)
                {
                    continue;
                }

                auto old_value = parameters->get(mapping.parameter_name);

                parameters->update(mapping.parameter_name,
                                   old_value < 0.5f ? 1.f : 0.f);
                break;
            }
        }
    };

private:
    std::shared_ptr<AudioParametersT> parameters;
    std::unique_ptr<persistence::Crud<std::span<uint8_t>>> storage;
    MapType mappings;

    static constexpr char TAG[] = "midi_mapping";
};

} // namespace shrapnel::midi
