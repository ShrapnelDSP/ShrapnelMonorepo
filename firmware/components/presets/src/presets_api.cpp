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

#include "presets_api.h"
#include "presets.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

namespace shrapnel::api {

template <>
int to_proto(const presets::ParametersData &message,
             shrapnel_presets_PresetParameters &out)
{
    out.amp_gain = message.amp_gain * 1000;
    out.amp_channel = message.amp_channel * 1000;
    out.bass = message.bass * 1000;
    out.middle = message.middle * 1000;
    out.treble = message.treble * 1000;
    out.contour = message.contour * 1000;
    out.volume = message.volume * 1000;
    out.noise_gate_threshold = message.noise_gate_threshold * 1000;
    out.noise_gate_hysteresis = message.noise_gate_hysteresis * 1000;
    out.noise_gate_attack = message.noise_gate_attack * 1000;
    out.noise_gate_hold = message.noise_gate_hold * 1000;
    out.noise_gate_release = message.noise_gate_release * 1000;
    out.noise_gate_bypass = message.noise_gate_bypass * 1000;
    out.chorus_rate = message.chorus_rate * 1000;
    out.chorus_depth = message.chorus_depth * 1000;
    out.chorus_mix = message.chorus_mix * 1000;
    out.chorus_bypass = message.chorus_bypass * 1000;
    out.wah_position = message.wah_position * 1000;
    out.wah_vocal = message.wah_vocal * 1000;
    out.wah_bypass = message.wah_bypass * 1000;
    return 0;
}

template <>
int from_proto(const shrapnel_presets_PresetParameters &message,
               presets::ParametersData &out)
{
    out.amp_gain = message.amp_gain / 1000.f;
    out.amp_channel = message.amp_channel / 1000.f;
    out.bass = message.bass / 1000.f;
    out.middle = message.middle / 1000.f;
    out.treble = message.treble / 1000.f;
    out.contour = message.contour / 1000.f;
    out.volume = message.volume / 1000.f;
    out.noise_gate_threshold = message.noise_gate_threshold / 1000.f;
    out.noise_gate_hysteresis = message.noise_gate_hysteresis / 1000.f;
    out.noise_gate_attack = message.noise_gate_attack / 1000.f;
    out.noise_gate_hold = message.noise_gate_hold / 1000.f;
    out.noise_gate_release = message.noise_gate_release / 1000.f;
    out.noise_gate_bypass = message.noise_gate_bypass / 1000.f;
    out.chorus_rate = message.chorus_rate / 1000.f;
    out.chorus_depth = message.chorus_depth / 1000.f;
    out.chorus_mix = message.chorus_mix / 1000.f;
    out.chorus_bypass = message.chorus_bypass / 1000.f;
    out.wah_position = message.wah_position / 1000.f;
    out.wah_vocal = message.wah_vocal / 1000.f;
    out.wah_bypass = message.wah_bypass / 1000.f;
    return 0;
}

template <>
int to_proto(const presets::PresetData &preset, shrapnel_presets_Preset &out)
{
    int rc = to_proto<shrapnel_presets_PresetParameters>(preset.parameters,
                                                         out.parameters);
    if(rc != 0)
    {
        return -1;
    }
    out.has_parameters = true;

    strncpy(out.name, preset.name.data(), sizeof out.name);

    return 0;
}

template <>
int from_proto(const shrapnel_presets_Preset &unpacked,
               presets::PresetData &out)
{
    out.name = unpacked.name;
    return from_proto<presets::ParametersData>(unpacked.parameters,
                                               out.parameters);
}

template <>
int to_proto(const presets::Initialise &, shrapnel_presets_Initialise &)
{
    return 0;
}

template <>
int from_proto(const shrapnel_presets_Initialise &, presets::Initialise &)
{
    return 0;
}

template <>
int to_proto(const presets::Notify &message, shrapnel_presets_Notify &out)
{
    out.preset.id = message.id;
    out.has_preset = true;
    out.preset.has_preset = true;
    return to_proto<shrapnel_presets_Preset>(message.preset, out.preset.preset);
}

template <>
int from_proto(const shrapnel_presets_Notify &message, presets::Notify &out)
{
    out.id = message.preset.id;
    return from_proto<presets::PresetData>(message.preset.preset, out.preset);
}

template <>
int to_proto(const presets::Create &message, shrapnel_presets_Create &out)
{
    out.has_preset = true;
    return to_proto<shrapnel_presets_Preset>(message.preset, out.preset);
}

template <>
int from_proto(const shrapnel_presets_Create &message, presets::Create &out)
{
    return from_proto<presets::PresetData>(message.preset, out.preset);
}

template <>
int to_proto(const presets::Update &message, shrapnel_presets_Update &out)
{
    out.preset.id = message.id;
    out.has_preset = true;
    out.preset.has_preset = true;
    return to_proto<shrapnel_presets_Preset>(message.preset, out.preset.preset);
}

template <>
int from_proto(const shrapnel_presets_Update &message, presets::Update &out)
{
    out.id = message.preset.id;
    return from_proto<presets::PresetData>(message.preset.preset, out.preset);
}

template <>
int to_proto(const presets::Delete &message, shrapnel_presets_Remove &out)
{
    out.id = message.id;
    return 0;
}

template <>
int from_proto(const shrapnel_presets_Remove &message, presets::Delete &out)
{
    out.id = message.id;
    return 0;
}

template <>
int to_proto(const presets::PresetsApiMessage &message,
             shrapnel_presets_Message &out)
{
    return std::visit(
        [&out](const auto &message) -> int
        {
            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, presets::Initialise>)
            {
                out.which_message = shrapnel_presets_Message_initialise_tag;
                int rc = to_proto<shrapnel_presets_Initialise>(
                    message, out.message.initialise);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, presets::Notify>)
            {
                out.which_message = shrapnel_presets_Message_notify_tag;
                int rc = to_proto<shrapnel_presets_Notify>(message,
                                                           out.message.notify);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, presets::Create>)
            {
                out.which_message = shrapnel_presets_Message_create_tag;
                int rc = to_proto<shrapnel_presets_Create>(message,
                                                           out.message.create);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, presets::Update>)
            {
                out.which_message = shrapnel_presets_Message_update_tag;
                int rc = to_proto<shrapnel_presets_Update>(message,
                                                           out.message.update);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, presets::Delete>)
            {
                out.which_message = shrapnel_presets_Message_remove_tag;
                int rc = to_proto<shrapnel_presets_Remove>(message,
                                                           out.message.remove);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }

            return 0;
        },
        message);
}

template <>
int from_proto(const shrapnel_presets_Message &message,
               presets::PresetsApiMessage &out)
{
    switch(message.which_message)
    {
    case shrapnel_presets_Message_initialise_tag:
    {
        presets::Initialise tmp{};
        from_proto<presets::Initialise>(message.message.initialise, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_presets_Message_notify_tag:
    {
        presets::Notify tmp{};
        from_proto<presets::Notify>(message.message.notify, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_presets_Message_create_tag:
    {
        presets::Create tmp{};
        from_proto<presets::Create>(message.message.create, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_presets_Message_update_tag:
    {
        presets::Update tmp{};
        from_proto<presets::Update>(message.message.update, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_presets_Message_remove_tag:
    {
        presets::Delete tmp{};
        from_proto<presets::Delete>(message.message.remove, tmp);
        out = tmp;
        return 0;
    }
    }

    return -1;
}

template <>
std::optional<std::span<uint8_t>> to_bytes(const shrapnel_presets_Preset &proto,
                                           std::span<uint8_t> buffer)
{
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    bool success = pb_encode(&stream, &shrapnel_presets_Preset_msg, &proto);
    if(!success)
    {
        return std::nullopt;
    }

    return buffer.subspan(0, stream.bytes_written);
}

template <>
std::optional<shrapnel_presets_Preset>
from_bytes(std::span<const uint8_t> buffer)
{
    auto stream = pb_istream_from_buffer(buffer.data(), buffer.size());
    shrapnel_presets_Preset unpacked = shrapnel_presets_Preset_init_zero;
    bool success = pb_decode(&stream, &shrapnel_presets_Preset_msg, &unpacked);
    if(!success)
    {
        return std::nullopt;
    }

    return unpacked;
}

template <>
std::optional<std::span<uint8_t>> to_bytes(const presets::PresetData &message,
                                           std::span<uint8_t> buffer)
{
    shrapnel_presets_Preset proto = shrapnel_presets_Preset_init_zero;
    int rc = to_proto(message, proto);
    if(rc != 0)
    {
        return std::nullopt;
    }

    return to_bytes(proto, buffer);
}

template <>
std::optional<presets::PresetData> from_bytes(std::span<const uint8_t> buffer)
{
    auto proto = from_bytes<shrapnel_presets_Preset>(buffer);
    if(!proto.has_value())
    {
        return std::nullopt;
    }

    presets::PresetData out{};
    int rc = from_proto<presets::PresetData>(*proto, out);
    if(rc != 0)
    {
        return std::nullopt;
    }

    return out;
}

} // namespace shrapnel::api