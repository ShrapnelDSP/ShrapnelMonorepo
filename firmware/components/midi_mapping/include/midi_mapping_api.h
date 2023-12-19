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
#include "midi_mapping.h"
#include "midi_mapping.pb.h"
#include <utility>
#include <variant>

namespace shrapnel {
namespace midi {

struct GetRequest
{
    std::strong_ordering operator<=>(const GetRequest &other) const = default;
};

struct GetResponse
{
    etl::map<Mapping::id_t, Mapping, 10> mappings;
    std::strong_ordering operator<=>(const GetResponse &other) const = default;
};

struct CreateRequest
{
    std::pair<Mapping::id_t, Mapping> mapping;
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
                                       GetResponse,
                                       CreateRequest,
                                       CreateResponse,
                                       Update,
                                       Remove,
                                       MessageReceived>;

etl::string_stream &operator<<(etl::string_stream &out, const Mapping &self);
etl::string_stream &operator<<(etl::string_stream &out, const GetRequest &self);
etl::string_stream &operator<<(etl::string_stream &out,
                               const GetResponse &self);
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
std::optional<std::span<uint8_t>>
to_bytes(const shrapnel_midi_mapping_Message &message,
         std::span<uint8_t> buffer);

template <>
std::optional<shrapnel_midi_mapping_Message>
from_bytes(std::span<const uint8_t> buffer);

template <>
std::optional<std::span<uint8_t>>
to_bytes(const shrapnel_midi_mapping_MappingList &message,
         std::span<uint8_t> buffer);

template <>
std::optional<shrapnel_midi_mapping_MappingList>
from_bytes(std::span<const uint8_t> buffer);

template <>
std::optional<shrapnel_midi_mapping_Mapping>
to_proto(const midi::Mapping &message);

template <>
std::optional<midi::Mapping>
from_proto(const shrapnel_midi_mapping_Mapping &message);

template <>
std::optional<shrapnel_midi_mapping_MappingRecord>
to_proto(const std::pair<midi::Mapping::id_t, midi::Mapping> &message);

template <>
std::optional<std::pair<midi::Mapping::id_t, midi::Mapping>>
from_proto(const shrapnel_midi_mapping_MappingRecord &message);

template <>
std::optional<shrapnel_midi_mapping_MappingList>
to_proto(const etl::map<midi::Mapping::id_t, midi::Mapping, 10> &message);

template <>
std::optional<etl::map<midi::Mapping::id_t, midi::Mapping, 10>>
from_proto(const shrapnel_midi_mapping_MappingList &message);

template <>
std::optional<shrapnel_midi_mapping_GetRequest>
to_proto(const midi::GetRequest &message);

template <>
std::optional<midi::GetRequest>
from_proto(const shrapnel_midi_mapping_GetRequest &message);

template <>
std::optional<shrapnel_midi_mapping_GetResponse>
to_proto(const midi::GetResponse &message);

template <>
std::optional<midi::GetResponse>
from_proto(const shrapnel_midi_mapping_GetResponse &message);

template <>
std::optional<shrapnel_midi_mapping_CreateRequest>
to_proto(const midi::CreateRequest &message);

template <>
std::optional<midi::CreateRequest>
from_proto(const shrapnel_midi_mapping_CreateRequest &message);

template <>
std::optional<shrapnel_midi_mapping_CreateResponse>
to_proto(const midi::CreateResponse &message);

template <>
std::optional<midi::CreateResponse>
from_proto(const shrapnel_midi_mapping_CreateResponse &message);

template <>
std::optional<shrapnel_midi_mapping_Update>
to_proto(const midi::Update &message);

template <>
std::optional<midi::Update>
from_proto(const shrapnel_midi_mapping_Update &message);

template <>
std::optional<shrapnel_midi_mapping_Remove>
to_proto(const midi::Remove &message);

template <>
std::optional<midi::Remove>
from_proto(const shrapnel_midi_mapping_Remove &message);

template <>
std::optional<shrapnel_midi_mapping_MidiMessage >
to_proto(const midi::Message &message);

template <>
std::optional<midi::Message>
from_proto(const shrapnel_midi_mapping_MidiMessage &message);

template <>
std::optional<shrapnel_midi_mapping_MidiMessage_NoteOn >
to_proto(const midi::Message::NoteOn &message);

template <>
std::optional<midi::Message::NoteOn>
from_proto(const shrapnel_midi_mapping_MidiMessage_NoteOn &message);

template <>
std::optional<shrapnel_midi_mapping_MidiMessage_NoteOff >
to_proto(const midi::Message::NoteOff &message);

template <>
std::optional<midi::Message::NoteOff>
from_proto(const shrapnel_midi_mapping_MidiMessage_NoteOff &message);

template <>
std::optional<shrapnel_midi_mapping_MidiMessage_ControlChange >
to_proto(const midi::Message::ControlChange &message);

template <>
std::optional<midi::Message::ControlChange>
from_proto(const shrapnel_midi_mapping_MidiMessage_ControlChange &message);

template <>
std::optional<shrapnel_midi_mapping_MidiMessage_ProgramChange >
to_proto(const midi::Message::ProgramChange &message);

template <>
std::optional<midi::Message::ProgramChange>
from_proto(const shrapnel_midi_mapping_MidiMessage_ProgramChange &message);

template <>
std::optional<shrapnel_midi_mapping_MessageReceived>
to_proto(const midi::MessageReceived &message);

template <>
std::optional<midi::MessageReceived>
from_proto(const shrapnel_midi_mapping_MessageReceived &message);

template <>
std::optional<shrapnel_midi_mapping_Message>
to_proto(const midi::MappingApiMessage &message);

template <>
std::optional<midi::MappingApiMessage>
from_proto(const shrapnel_midi_mapping_Message &message);

} // namespace api
} // namespace shrapnel
