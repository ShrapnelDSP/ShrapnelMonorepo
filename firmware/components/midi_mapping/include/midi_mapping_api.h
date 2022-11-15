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

#include "audio_param.h"
#include "midi_mapping.h"
#include <variant>
#include <utility>

namespace shrapnel {
namespace midi {

struct GetRequest {};

struct GetResponse {
    //etl::imap mappings,
    std::strong_ordering operator<=>(const GetResponse &other) const = default;
};

struct CreateRequest {
    std::pair<Mapping::id_t, Mapping> mapping;
    std::strong_ordering operator<=>(const CreateRequest &other) const = default;

};

struct CreateResponse {
    std::pair<Mapping::id_t, Mapping> mapping;
    std::strong_ordering operator<=>(const CreateResponse &other) const = default;
};

struct Update {
    std::pair<Mapping::id_t, Mapping> mapping;
    std::strong_ordering operator<=>(const Update &other) const = default;
};

struct Remove {
    Mapping::id_t id;
    std::strong_ordering operator<=>(const Remove &other) const = default;
};

using MappingApiMessage = std::variant<GetRequest, GetResponse, CreateRequest, CreateResponse, Update, Remove>;


etl::string_stream& operator<<(etl::string_stream&  out, const GetRequest& self);
etl::string_stream& operator<<(etl::string_stream&  out, const GetResponse& self);
etl::string_stream& operator<<(etl::string_stream&  out, const CreateRequest& self);
etl::string_stream& operator<<(etl::string_stream&  out, const CreateResponse& self);
etl::string_stream& operator<<(etl::string_stream&  out, const Update& self);
etl::string_stream& operator<<(etl::string_stream&  out, const Remove& self);
etl::string_stream& operator<<(etl::string_stream&  out, const MappingApiMessage& self);

}
}
