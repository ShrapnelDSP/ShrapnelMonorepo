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

#include "selected_preset_json_parser.h"
#include "esp_log.h"
#include "selected_preset_message_type.h"
#include <etl/map.h>

namespace shrapnel::selected_preset {

template <>
std::optional<Read> from_json(const rapidjson::Value &json)
{
    return Read{};
}

template <>
std::optional<Write> from_json(const rapidjson::Value &json)
{
    auto id_json = json.FindMember("selectedPreset");
    if(id_json == json.MemberEnd())
    {
        return std::nullopt;
    }

    if(!id_json->value.IsUint())
    {
        return std::nullopt;
    }

    return Write{.selectedPresetId{id_json->value.GetUint()}};
}

template <>
std::optional<SelectedPresetApiMessage> from_json(const rapidjson::Value &json)
{
    constexpr char TAG[] = "SelectedPresetApiMessage from_json";

    auto message_type_member = json.FindMember("messageType");
    if(message_type_member == json.MemberEnd())
    {
        ESP_LOGE(TAG, "messageType is missing");
        goto error;
    }

    if(!message_type_member->value.IsString())
    {
        ESP_LOGE(TAG, "messageType is not string");
        goto error;
    }

    {
        const etl::map<std::string,
                       std::function<std::optional<SelectedPresetApiMessage>()>,
                       2>
            lut{
                {get_message_type<Read>(),
                 [&] { return from_json<Read>(json); }},
                {get_message_type<Write>(),
                 [&] { return from_json<Write>(json); }},
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

} // namespace shrapnel::selected_preset
