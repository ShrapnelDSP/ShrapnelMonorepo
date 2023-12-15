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

#include "presets_json_builder.h"
#include "preset_serialisation.h"
#include "presets.pb-c.h"
#include "presets_message_type.h"
#include "uuid.h"
#include <cppcodec/base64_default_rfc4648.hpp>
#include <esp_log.h>
#include <span>

namespace shrapnel::presets {

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const ParametersData &object)
{
    uint8_t buffer[100] = {};
    auto data = std::span<uint8_t, std::dynamic_extent>(buffer);

    int rc = serialise_parameters(object, data);
    if(rc != 0)
    {
        return {/* null */};
    }

    using base64_codec = cppcodec::base64_rfc4648;

    std::array<char, 200> base64_buffer;
    auto encoded_size = base64_codec::encoded_size(data.size());
    if(encoded_size > base64_buffer.size())
    {
        return {/* null */};
    }

    auto error = base64_codec::encode(base64_buffer.data(),
                                      base64_buffer.size(),
                                      data.data(),
                                      data.size(),
                                      encoded_size);

    if(!std::holds_alternative<std::monostate>(error))
    {
        return {/* null */};
    }

    auto json = rapidjson::Value{};
    json.SetString(base64_buffer.data(), encoded_size, document.GetAllocator());
    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const PresetData &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value name{object.name.c_str(), document.GetAllocator()};
    json.AddMember("name", name, document.GetAllocator());
    json.AddMember("parameters",
                   to_json(document, object.parameters),
                   document.GetAllocator());

    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document, const Notify &object)
{
    rapidjson::Value json;
    json.SetObject();

    json.AddMember<unsigned int>("id", object.id, document.GetAllocator());
    json.AddMember(
        "preset", to_json(document, object.preset), document.GetAllocator());

    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const PresetsApiMessage &object)
{
    rapidjson::Value json;

    auto visitor = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, Notify>)
        {
            json = to_json(document, message);
        }

        json.AddMember("messageType",
                       rapidjson::StringRef(get_message_type<T>()),
                       document.GetAllocator());
    };

    std::visit(visitor, object);

    return json;
}

} // namespace shrapnel::presets
