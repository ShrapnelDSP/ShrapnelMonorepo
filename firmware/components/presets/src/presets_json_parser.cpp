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

#include "presets_json_parser.h"
#include "esp_log.h"
#include "presets.pb-c.h"
#include "presets_message_type.h"
#include <cppcodec/base64_rfc4648.hpp>
#include <etl/map.h>
#include <uuid.h>

namespace shrapnel::presets {

template <>
std::optional<ParametersData> from_json(const rapidjson::Value &json)
{
    using base64_codec = cppcodec::base64_rfc4648;
    if(!json.IsString())
    {
        return std::nullopt;
    }

    std::array<uint8_t, 100> buffer{};
    if(base64_codec::decoded_max_size(json.GetStringLength()) > buffer.size())
    {
        return std::nullopt;
    }

    size_t decoded_size = base64_codec::decode(
        buffer.data(), buffer.size(), json.GetString(), json.GetStringLength());

    auto unpacked =
        preset_parameters__unpack(nullptr, decoded_size, buffer.data());
    if(unpacked == nullptr)
    {
        return std::nullopt;
    }

    auto out = ParametersData{
        .amp_gain = unpacked->amp_gain / 1000.f,
        .amp_channel = unpacked->amp_channel / 1000.f,
        .bass = unpacked->bass / 1000.f,
        .middle = unpacked->middle / 1000.f,
        .treble = unpacked->treble / 1000.f,
        .contour = unpacked->contour / 1000.f,
        .volume = unpacked->volume / 1000.f,
        .noise_gate_threshold = unpacked->noise_gate_threshold / 1000.f,
        .noise_gate_hysteresis = unpacked->noise_gate_hysteresis / 1000.f,
        .noise_gate_attack = unpacked->noise_gate_attack / 1000.f,
        .noise_gate_hold = unpacked->noise_gate_hold / 1000.f,
        .noise_gate_release = unpacked->noise_gate_release / 1000.f,
        .noise_gate_bypass = unpacked->noise_gate_bypass / 1000.f,
        .chorus_rate = unpacked->chorus_rate / 1000.f,
        .chorus_depth = unpacked->chorus_depth / 1000.f,
        .chorus_mix = unpacked->chorus_mix / 1000.f,
        .chorus_bypass = unpacked->chorus_bypass / 1000.f,
        .wah_position = unpacked->wah_position / 1000.f,
        .wah_vocal = unpacked->wah_vocal / 1000.f,
        .wah_bypass = unpacked->wah_bypass / 1000.f,
    };

    preset_parameters__free_unpacked(unpacked, nullptr);
    return out;
}

template <>
std::optional<PresetData> from_json(const rapidjson::Value &json)
{
    auto name = json.FindMember("name");
    if(name == json.MemberEnd() || !name->value.IsString())
        return std::nullopt;

    auto parameters_json = json.FindMember("parameters");
    if(parameters_json == json.MemberEnd())
        return std::nullopt;
    auto parameters = from_json<ParametersData>(parameters_json->value);
    if(!parameters.has_value())
        return std::nullopt;

    return {{
        .name{name->value.GetString()},
        .parameters{*parameters},
    }};
}

template <>
std::optional<Initialise> from_json(const rapidjson::Value &json)
{
    return Initialise();
}

template <>
std::optional<Create> from_json(const rapidjson::Value &json)
{
    auto preset_json = json.FindMember("preset");
    if(preset_json == json.MemberEnd())
        return std::nullopt;

    auto preset = from_json<PresetData>(preset_json->value);
    if(!preset.has_value())
        return std::nullopt;

    return {{.preset = *preset}};
}

template <>
std::optional<Update> from_json(const rapidjson::Value &json)
{
    auto id_json = json.FindMember("id");
    if(id_json == json.MemberEnd())
        return std::nullopt;
    if(!id_json->value.IsUint())
        return std::nullopt;

    auto preset_json = json.FindMember("preset");
    if(preset_json == json.MemberEnd())
        return std::nullopt;
    auto preset = from_json<PresetData>(preset_json->value);
    if(!preset.has_value())
        return std::nullopt;

    return {{.id{id_json->value.GetUint()}, .preset = *preset}};
}

template <>
std::optional<Delete> from_json(const rapidjson::Value &json)
{
    auto id_json = json.FindMember("id");
    if(id_json == json.MemberEnd())
        return std::nullopt;

    if(!id_json->value.IsUint())
        return std::nullopt;

    return {{.id{id_json->value.GetUint()}}};
}

template <>
std::optional<PresetsApiMessage> from_json(const rapidjson::Value &json)
{
    constexpr char TAG[] = "PresetsApiMessage from_json";

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
                       std::function<std::optional<PresetsApiMessage>()>,
                       4>
            lut{
                {get_message_type<Initialise>(),
                 [&] { return from_json<Initialise>(json); }},
                {get_message_type<Create>(),
                 [&] { return from_json<Create>(json); }},
                {get_message_type<Update>(),
                 [&] { return from_json<Update>(json); }},
                {get_message_type<Delete>(),
                 [&] { return from_json<Delete>(json); }},
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

} // namespace shrapnel::presets
