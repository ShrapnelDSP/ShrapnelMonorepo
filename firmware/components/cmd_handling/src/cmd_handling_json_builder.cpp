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

#include "cmd_handling_json_builder.h"

#include "cmd_handling_api.h"
#include "cmd_handling_json.h"
#include "esp_log.h"

namespace shrapnel::parameters {

template <>
rapidjson::Value to_json(rapidjson::Document &document, const Update &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value id;
    id.SetString(object.id.data(), object.id.size(), document.GetAllocator());
    json.AddMember("id", id, document.GetAllocator());

    rapidjson::Value value;
    value = object.value;
    json.AddMember("value", value, document.GetAllocator());

    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const ApiMessage &object)
{
    static constexpr char TAG[] = "parameters::ApiMessage to json";
    rapidjson::Value json;

    auto visitor = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, Update>)
        {
            json = to_json(document, message);
        }
        else
        {
            ESP_LOGE(TAG, "No handler registered for message");
        }

        json.AddMember("messageType",
                       rapidjson::StringRef(get_message_type<T>()),
                       document.GetAllocator());
    };

    std::visit(visitor, object);

    return json;
}

} // namespace shrapnel::parameters
