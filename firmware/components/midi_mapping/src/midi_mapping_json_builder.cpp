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

#include "midi_mapping_json_builder.h"
#include "midi_mapping_api.h"
#include "midi_message_type.h"

namespace {
    constexpr char TAG[] = "midi_mapping_json_builder";
};

namespace shrapnel {
namespace midi {

template<>
rapidjson::Value to_json(rapidjson::Document &document, const Mapping &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value midi_channel;
    midi_channel = object.midi_channel;
    json.AddMember("midi_channel", midi_channel, document.GetAllocator());

    rapidjson::Value cc_number;
    cc_number = object.cc_number;
    json.AddMember("cc_number", cc_number, document.GetAllocator());

    rapidjson::Value parameter_id;
    parameter_id.SetString(object.parameter_name.data(), object.parameter_name.size(), document.GetAllocator());
    json.AddMember("parameter_id", parameter_id, document.GetAllocator());

    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const Mapping::id_t &object) {
    char uuid[37];
    sprintf(uuid,
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-"
            "%02x%02x%02x%02x%02x%02x",
            object[0], object[1], object[2], object[3], object[4], object[5],
            object[6], object[7], object[8], object[9], object[10], object[11],
            object[12], object[13], object[14], object[15]);

    rapidjson::Value out;
    out.SetString(uuid, 36, document.GetAllocator());

    return out;
}

template<>
rapidjson::Value to_json(rapidjson::Document &document, const std::pair<Mapping::id_t, Mapping> &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value mapping = to_json(document, object.second);
    auto id = to_json(document, object.first);
    json.AddMember(id, mapping, document.GetAllocator());
    return json;
}

template<>
rapidjson::Value to_json(rapidjson::Document &document, const CreateResponse &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value mapping = to_json(document, object.mapping);
    json.AddMember("mapping", mapping, document.GetAllocator());
    return json;
}

template<>
rapidjson::Value to_json(rapidjson::Document &document, const GetResponse &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value mappings = to_json(document, *object.mappings);
    json.AddMember("mappings", mappings, document.GetAllocator());
    return json;
}

template<>
rapidjson::Value to_json(rapidjson::Document &document, const MappingApiMessage &object)
{
    rapidjson::Value json;

    auto visitor = [&](const auto &message) {
        using T = std::decay_t<decltype(message)>;

        if constexpr (std::is_same_v<T, CreateResponse> ||
                      std::is_same_v<T, GetResponse>) {
            json = to_json(document, message);
        } else {
            ESP_LOGE(TAG, "No handler registered for message");
        }

        json.AddMember("messageType", rapidjson::StringRef(get_message_type<T>()), document.GetAllocator());
    };

    std::visit(visitor, object);

    return json;
}

template<>
rapidjson::Value to_json(rapidjson::Document &document, const etl::imap<Mapping::id_t, Mapping> &object)
{
    rapidjson::Value json;
    json.SetObject();

    for(const auto &entry : object)
    {
        rapidjson::Value mapping = to_json(document, entry.second);
        auto id = to_json(document, entry.first);
        json.AddMember(id, mapping, document.GetAllocator());
    }

    return json;
}

}
}
