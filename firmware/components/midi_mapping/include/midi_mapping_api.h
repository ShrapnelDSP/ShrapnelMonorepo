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

#include "api.h"
#include "audio_param.h"
#include "midi_mapping.pb.h"
#include "midi_protocol.h"
#include "presets.h"
#include <utility>
#include <variant>

namespace shrapnel {
namespace midi {

struct Mapping
{
    using id_t = uint32_t;

    enum class Mode
    {
        PARAMETER,
        TOGGLE,
        BUTTON,
    };

    uint8_t midi_channel;
    uint8_t cc_number;
    Mode mode;
    parameters::id_t parameter_name;
    presets::id_t preset_id;

    std::strong_ordering operator<=>(const Mapping &other) const = default;
};

struct GetRequest
{
    std::strong_ordering operator<=>(const GetRequest &other) const = default;
};

struct CreateRequest
{
    Mapping mapping;
    std::strong_ordering
    operator<=>(const CreateRequest &other) const = default;
};

struct CreateResponse
{
    std::pair<Mapping::id_t, Mapping> mapping;
    std::strong_ordering
    operator<=>(const CreateResponse &other) const = default;
};

struct Update

{
    std::pair<Mapping::id_t, Mapping> mapping;
    std::strong_ordering operator<=>(const Update &other) const = default;
};

struct Remove
{
    Mapping::id_t id;
    std::strong_ordering operator<=>(const Remove &other) const = default;
};

struct MessageReceived
{
    Message message;
    std::strong_ordering
    operator<=>(const MessageReceived &other) const = default;
};

using MappingApiMessage = std::variant<GetRequest,
                                       CreateRequest,
                                       CreateResponse,
                                       Update,
                                       Remove,
                                       MessageReceived>;

etl::string_stream &operator<<(etl::string_stream &out, const Mapping &self);
etl::string_stream &operator<<(etl::string_stream &out, const GetRequest &self);
etl::string_stream &operator<<(etl::string_stream &out,
                               const CreateRequest &self);
etl::string_stream &operator<<(etl::string_stream &out,
                               const CreateResponse &self);
etl::string_stream &operator<<(etl::string_stream &out, const Update &self);
etl::string_stream &operator<<(etl::string_stream &out, const Remove &self);
etl::string_stream &operator<<(etl::string_stream &out,
                               const MessageReceived &self);
etl::string_stream &operator<<(etl::string_stream &out,
                               const MappingApiMessage &self);

} // namespace midi

namespace api {

template <>
std::optional<std::span<uint8_t>> to_bytes(const midi::Mapping &message,
                                           std::span<uint8_t> buffer);

template <>
std::optional<midi::Mapping> from_bytes(std::span<const uint8_t> buffer);

template <>
int to_proto(const midi::MappingApiMessage &message,
             shrapnel_midi_mapping_Message &out);

template <>
int from_proto(const shrapnel_midi_mapping_Message &message,
               midi::MappingApiMessage &out);

template <>
std::optional<midi::Message> from_bytes(std::span<const uint8_t> buffer);

template <>
int from_proto(const shrapnel_midi_mapping_MidiMessage &message,
               midi::Message &out);

} // namespace api
} // namespace shrapnel
