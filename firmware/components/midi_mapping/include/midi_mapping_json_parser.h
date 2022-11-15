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

#include <cstring>

// Disable warning inside rapidjson
// https://github.com/Tencent/rapidjson/issues/1700
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include "rapidjson/document.h"
#pragma GCC diagnostic pop

#include "midi_mapping_api.h"
#include <optional>

namespace shrapnel {
namespace midi {

/// return non-zero on error
int parse_uuid(Mapping::id_t &uuid, const char *string);

template<typename T>
std::optional<T> from_json(const rapidjson::Value &json);

template<>
std::optional<GetRequest> from_json(const rapidjson::Value &json);

template<>
std::optional<Mapping> from_json(const rapidjson::Value &json);

template<>
std::optional<std::pair<Mapping::id_t, Mapping>> from_json(const rapidjson::Value &json);

template<>
std::optional<CreateRequest> from_json(const rapidjson::Value &json);

template<>
std::optional<Update> from_json(const rapidjson::Value &json);

template<>
std::optional<Remove> from_json(const rapidjson::Value &json);

/** Convert the message into a object representing it.
 */
template<>
std::optional<MappingApiMessage> from_json(const rapidjson::Value &json);

}
}
