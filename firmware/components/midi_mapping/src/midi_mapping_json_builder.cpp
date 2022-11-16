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

template<>
rapidjson::Value to_json(rapidjson::Document &document, const std::pair<Mapping::id_t, Mapping> &object)
{
    rapidjson::Value json;
    json.SetObject();

    char uuid[37];
    sprintf(uuid,
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-"
            "%02x%02x%02x%02x%02x%02x",
            object.first[0],
            object.first[1],
            object.first[2],
            object.first[3],
            object.first[4],
            object.first[5],
            object.first[6],
            object.first[7],
            object.first[8],
            object.first[9],
            object.first[10],
            object.first[11],
            object.first[12],
            object.first[13],
            object.first[14],
            object.first[15]);

    rapidjson::Value mapping = to_json(document, object.second);
    rapidjson::Value uuid_json;
    uuid_json.SetString(uuid, 36, document.GetAllocator());
    json.AddMember(uuid_json, mapping, document.GetAllocator());
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

    rapidjson::Value mappings = to_json(document, object.mappings);
    json.AddMember("mappings", mappings, document.GetAllocator());
    return json;
}

template<>
rapidjson::Value to_json(rapidjson::Document &document, const MappingApiMessage &object)
{
    rapidjson::Value json;
    json.SetObject();

    // TODO any way to make this DRY? We need to call to_json with the correct
    // type, and add the messageType field with the correct value
    auto visitor = [&](const auto &message) {
        using T = std::decay_t<decltype(message)>;

        if constexpr (std::is_same_v<T, CreateResponse>) {
            auto message_json = to_json(document, message);
            json.Swap(message_json);
            json.AddMember("messageType", "MidiMap::create::response", document.GetAllocator());
        } else if constexpr (std::is_same_v<T, GetResponse>) {
            auto message_json = to_json(document, message);
            json.Swap(message_json);
            json.AddMember("messageType", "MidiMap::get::response", document.GetAllocator());
        } else {
            ESP_LOGE(TAG, "No handler registered for message");
        }
    };

    std::visit(visitor, object);
    return json;
}

}
}
