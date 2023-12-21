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
    // FIXME: just one entry for now to work around not enough memory
    // long term this can be fixed by sending one mapping at a time
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
to_bytes(const etl::map<midi::Mapping::id_t, midi::Mapping, 10> &message,
         std::span<uint8_t> buffer);

template <>
std::optional<etl::map<midi::Mapping::id_t, midi::Mapping, 10>>
from_bytes(std::span<const uint8_t> buffer);

// FIXME: remove and use the version above
template <>
std::optional<std::span<uint8_t>>
to_bytes(const shrapnel_midi_mapping_MappingList &message,
         std::span<uint8_t> buffer);

// FIXME: remove
template <>
int
to_proto(const etl::map<midi::Mapping::id_t, midi::Mapping, 10> &message, shrapnel_midi_mapping_MappingList &out);

// FIXME: remove
template <>
int
from_proto(const shrapnel_midi_mapping_MappingList &message, etl::map<midi::Mapping::id_t, midi::Mapping, 10> &out);

template <>
int
to_proto(const midi::MappingApiMessage &message, shrapnel_midi_mapping_Message &out);

template <>
int
from_proto(const shrapnel_midi_mapping_Message &message, midi::MappingApiMessage &out);

} // namespace api
} // namespace shrapnel
