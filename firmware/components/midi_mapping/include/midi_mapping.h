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
 * \page ws_api websockets API
 *
 * This page documents the message passing API used to communicate
 * between the Flutter frontend and the embedded firmware. The API works by
 * passing messages across a websockets connection. This allows the firmware to
 * send events to the frontend without polling. It is useful e.g. when an audio
 * parameter is updated through MIDI, and the new value should be displayed in
 * the UI.
 *
 * Messages may receive no response, a single response message or multiple
 * responses messages. This is documented separately for each message.
 *
 * \note The messages are encoded using the protobuf library. Protobuf is the
 *       most popular library for this, and has extensive community libraries
 *       that simply converting between messages and bytes.
 *
 * See also:
 * - Documentation in \p .proto files defining the messages
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

    MappingManager(
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
