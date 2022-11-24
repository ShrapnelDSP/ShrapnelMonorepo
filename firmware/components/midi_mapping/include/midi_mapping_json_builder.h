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

// Disable warning inside rapidjson
// https://github.com/Tencent/rapidjson/issues/1700
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#pragma GCC diagnostic pop

#include "midi_mapping_api.h"

namespace shrapnel {
namespace midi {

/** Convert \p object to a JSON value
 *
 * \note The \p document must not by modified by this function. It is only
 *       passed in so that its allocator member can be accessed.
 */
template<typename T>
rapidjson::Value to_json(rapidjson::Document &document, const T &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const Mapping &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const std::pair<Mapping::id_t, Mapping> &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const CreateResponse &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const GetResponse &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const MappingApiMessage &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const etl::imap<Mapping::id_t, Mapping> &object);

}
}
