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

#include "cmd_handling_json.h"
#include "cmd_handling_api.h"
#include "esp_log.h"

namespace shrapnel::parameters {

template<>
std::optional<Update> from_json(const rapidjson::Value &json)
{
    static constexpr char TAG[] = "parameters update from json";

    auto id = json.FindMember("id");
    if(id == json.MemberEnd()) {
        ESP_LOGE(TAG, "id is missing");
        return std::nullopt;
    }

    auto value = json.FindMember("value");
    if(id == json.MemberEnd()) {
        ESP_LOGE(TAG, "value is missing");
        return std::nullopt;
    }

    return Update{
        parameters::id_t(id->value.GetString()),
        static_cast<float>(value->value.GetDouble()),
    };
}

template<>
std::optional<Initialise> from_json(const rapidjson::Value &)
{
    return Initialise{};
}


template<>
std::optional<ApiMessage> from_json(const rapidjson::Value &json)
{
    static constexpr char TAG[] = "parameters api message from json";

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
        const etl::map<std::string, std::function<std::optional<ApiMessage>()>, 2> lut{
            { get_message_type<Update>(), [&]{return from_json<Update>(json);} },
            { get_message_type<Initialise>(), [&]{return from_json<Initialise>(json);} },
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
