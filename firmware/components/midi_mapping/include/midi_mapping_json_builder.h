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

template<std::size_t MAX_SIZE>
rapidjson::Value to_json(rapidjson::Document &document, const etl::map<Mapping::id_t, Mapping, MAX_SIZE> &object)
{
    rapidjson::Value json;
    json.SetObject();

    for(const auto &entry : object)
    {
        char uuid[37];
        sprintf(uuid,
                "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-"
                "%02x%02x%02x%02x%02x%02x",
                entry.first[0],
                entry.first[1],
                entry.first[2],
                entry.first[3],
                entry.first[4],
                entry.first[5],
                entry.first[6],
                entry.first[7],
                entry.first[8],
                entry.first[9],
                entry.first[10],
                entry.first[11],
                entry.first[12],
                entry.first[13],
                entry.first[14],
                entry.first[15]);


        rapidjson::Value mapping = to_json(document, entry.second);
        rapidjson::Value uuid_json;
        uuid_json.SetString(uuid, 36, document.GetAllocator());
        json.AddMember(uuid_json, mapping, document.GetAllocator());
    }

    return json;
}

}
}
