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

#include "uuid.h"

#include "esp_log.h"
#include <cstring>
#include <optional>

namespace shrapnel::uuid {

static int parse_uuid(uuid_t &uuid, const char *string);

template <>
std::optional<uuid_t> from_json(const rapidjson::Value &json)
{
    constexpr const char *TAG = "Mapping::id_t from_json";

    if(!json.IsString())
    {
        ESP_LOGE(TAG, "id is not string");
        return std::nullopt;
    }

    uuid_t out;
    int rc = parse_uuid(out, json.GetString());
    if(rc != 0)
    {
        ESP_LOGE(TAG, "failed to get uuid");
        return std::nullopt;
    }

    return out;
}

static int parse_uuid(uuid_t &uuid, const char *string)
{
    constexpr std::size_t UUID_LENGTH = 36;
    constexpr char TAG[] = "parse_uuid";

    if(UUID_LENGTH != std::strlen(string))
    {
        ESP_LOGE(TAG, "Incorrect UUID string length");
        return -1;
    }

    size_t i = 0;
    size_t j = 0;
    ESP_LOGD(TAG, "i = %zu, j = %zu", i, j);
    while(i < UUID_LENGTH)
    {
        char digit[2];
        std::memcpy(digit, &string[i], 2);

        ESP_LOGD(TAG, "digit = %c%c", digit[0], digit[1]);

        // TODO error on invalid characters: z, symbols etc
        //      The only valid characters are 0 to 9 and a to f.
        //
        //      Handle uppercase as well. It is required when a string UUID
        //      is an input as per RFC 4122 Section 3
        //      https://tools.ietf.org/html/rfc4122#section-3
        auto get_value = [&](char hex) -> uint8_t
        {
            if(hex >= 'a')
            {
                return hex - 'a' + 10;
            }
            else
            {
                return hex - '0';
            }
        };

        uuid[j] = get_value(digit[0]) << 4 | get_value(digit[1]);

        j++;

        i += 2;
        bool is_separator = (i == 8) || (i == 13) || (i == 18) || (i == 23);
        if(is_separator)
            i++;
        ESP_LOGD(TAG, "i = %zu, j = %zu", i, j);
    }

    return 0;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document, const uuid_t &object)
{
    char uuid[37];
    sprintf(uuid,
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-"
            "%02x%02x%02x%02x%02x%02x",
            object[0],
            object[1],
            object[2],
            object[3],
            object[4],
            object[5],
            object[6],
            object[7],
            object[8],
            object[9],
            object[10],
            object[11],
            object[12],
            object[13],
            object[14],
            object[15]);

    rapidjson::Value out;
    out.SetString(uuid, 36, document.GetAllocator());

    return out;
}

} // namespace shrapnel::uuid