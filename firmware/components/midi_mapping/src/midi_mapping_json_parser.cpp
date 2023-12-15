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

#include "midi_mapping_json_parser.h"
#include "midi_message_type.h"

namespace shrapnel {
namespace midi {

template<>
std::optional<GetRequest> from_json(const rapidjson::Value &)
{
    return GetRequest();
}

template<>
std::optional<Mapping> from_json(const rapidjson::Value &json)
{
    constexpr char TAG[] = "Mapping from_json";
    auto midi_channel = json.FindMember("midi_channel");
    if(midi_channel == json.MemberEnd()) {
        ESP_LOGE(TAG, "midi_channel is missing");
        return std::nullopt;
    }

    auto cc_number = json.FindMember("cc_number");
    if(cc_number == json.MemberEnd()) {
        ESP_LOGE(TAG, "cc_number is missing");
        return std::nullopt;
    }

    Mapping::Mode mode = Mapping::Mode::PARAMETER;
    auto mode_json = json.FindMember("mode");
    if(mode_json == json.MemberEnd())
    {
        ESP_LOGW(TAG, "mode is missing, setting it to PARAMETER");
    }
    else
    {
        if(!mode_json->value.IsString())
        {
            ESP_LOGE(TAG, "mode is not string");
            return std::nullopt;
        }

        static const etl::map<etl::string_view, Mapping::Mode, 3> lut{
            {"parameter", Mapping::Mode::PARAMETER},
            {"toggle", Mapping::Mode::TOGGLE},
            {"button", Mapping::Mode::BUTTON},
        };

        if(!lut.contains(mode_json->value.GetString()))
        {
            ESP_LOGE(TAG, "unknown mode %s", mode_json->value.GetString());
            return std::nullopt;
        }
        mode = lut.at(mode_json->value.GetString());
    }

    auto parameter_id_json = json.FindMember("parameter_id");
    std::optional<parameters::id_t> parameter_id;
    if(parameter_id_json != json.MemberEnd()) {
        parameter_id = parameter_id_json->value.GetString();
    }

    auto preset_id_json = json.FindMember("preset_id");
    std::optional<int> preset_id;
    if(preset_id_json != json.MemberEnd()) {
        preset_id = preset_id_json->value.GetInt();
    }

    // TODO range check before narrowing conversion to uint8_t
    return Mapping{
        static_cast<uint8_t>(midi_channel->value.GetInt()),
        static_cast<uint8_t>(cc_number->value.GetInt()),
        mode,
        parameter_id,
        preset_id,
    };
}

template<>
std::optional<std::pair<Mapping::id_t, Mapping>> from_json(const rapidjson::Value &json)
{
    constexpr char TAG[] = "pair<mapping::id, mapping> from_json";

    if(!json.IsObject())
    {
        ESP_LOGE(TAG, "mapping is not object");
        return std::nullopt;
    }

    // XXX There should be only one key, so we take the first one
    //     and ignore the rest
    auto mapping_entry_member = json.GetObject().begin();

    if(mapping_entry_member == json.GetObject().end())
    {
        ESP_LOGE(TAG, "mapping is empty");
        return std::nullopt;
    }

    auto &mapping_id = mapping_entry_member->name;
    auto &mapping_entry = mapping_entry_member->value;

    auto mapping = from_json<Mapping>(mapping_entry);
    if(!mapping.has_value()) {
        ESP_LOGE(TAG, "failed to get mapping");
        return std::nullopt;
    }

    auto id = uuid::from_json<Mapping::id_t>(mapping_id);
    if(!id.has_value()) {
        ESP_LOGE(TAG, "failed to get id");
        return std::nullopt;
    }

    return {{ *id, *mapping }};
}

template<>
std::optional<CreateRequest> from_json(const rapidjson::Value &json)
{
    constexpr char TAG[] = "Update from_json";

    auto mapping_member = json.FindMember("mapping");
    if(mapping_member == json.MemberEnd()) {
        ESP_LOGE(TAG, "mapping is missing");
        return std::nullopt;
    }

    auto mapping = from_json<std::pair<Mapping::id_t, Mapping>>(mapping_member->value);
    return CreateRequest{*mapping};
}

template<>
std::optional<Update> from_json(const rapidjson::Value &json)
{
    constexpr char TAG[] = "Update from_json";

    auto mapping_member = json.FindMember("mapping");
    if(mapping_member == json.MemberEnd()) {
        ESP_LOGE(TAG, "mapping is missing");
        return std::nullopt;
    }

    auto mapping = from_json<std::pair<Mapping::id_t, Mapping>>(mapping_member->value);
    return Update{*mapping};
}

template<>
std::optional<Remove> from_json(const rapidjson::Value &json) {
    constexpr char TAG[] = "Remove from_json";

    auto id_member = json.FindMember("id");
    if(id_member == json.MemberEnd()) {
        ESP_LOGE(TAG, "id is missing");
        return std::nullopt;
    }

    auto uuid = uuid::from_json<Mapping::id_t>(id_member->value);
    if(!uuid.has_value())
    {
        ESP_LOGE(TAG, "Failed to parse UUID");
        return std::nullopt;
    }

    return {{*uuid}};
}

template<>
std::optional<MappingApiMessage> from_json(const rapidjson::Value &json) {
    constexpr char TAG[] = "MidiApiMessage from_json";

    auto message_type_member = json.FindMember("messageType");
    if(message_type_member == json.MemberEnd()) {
        ESP_LOGE(TAG, "messageType is missing");
        goto error;
    }

    if(!message_type_member->value.IsString()) {
        ESP_LOGE(TAG, "messageType is not string");
        goto error;
    }

    {
        const etl::map<std::string, std::function<std::optional<MappingApiMessage>()>, 4> lut{
            { get_message_type<GetRequest>(), [&]{return from_json<GetRequest>(json);} },
            { get_message_type<CreateRequest>(), [&]{return from_json<CreateRequest>(json);} },
            { get_message_type<Update>(), [&]{return from_json<Update>(json);} },
            { get_message_type<Remove>(), [&]{return from_json<Remove>(json);} },
        };

        const char *message_type = message_type_member->value.GetString();
        if(auto f = lut.find(message_type); f != lut.end())
        {
            return f->second();
        }
    }

error:
    return std::nullopt;
}

}
}
