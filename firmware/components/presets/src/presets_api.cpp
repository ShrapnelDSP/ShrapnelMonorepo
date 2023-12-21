#include "presets_api.h"
#include "presets.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

namespace shrapnel::api {

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
std::optional<std::span<uint8_t>>
to_bytes(const shrapnel_presets_Message &message, std::span<uint8_t> buffer)
{
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    bool success = pb_encode(&stream, &shrapnel_presets_Message_msg, &message);
    if(!success)
    {
        return std::nullopt;
    }

    return buffer.subspan(0, stream.bytes_written);
}

template <>
std::optional<shrapnel_presets_Message>
from_bytes(std::span<const uint8_t> buffer)
{
    auto stream = pb_istream_from_buffer(buffer.data(), buffer.size());
    shrapnel_presets_Message unpacked = shrapnel_presets_Message_init_zero;
    bool success = pb_decode(&stream, &shrapnel_presets_Message_msg, &unpacked);
    if(!success)
    {
        return std::nullopt;
    }

    return unpacked;
}

template <>
int to_proto(const presets::ParametersData &parameters,
             shrapnel_presets_PresetParameters &out)
{
    out.amp_gain = parameters.amp_gain * 1000;
    out.amp_channel = parameters.amp_channel * 1000;
    out.bass = parameters.bass * 1000;
    out.middle = parameters.middle * 1000;
    out.treble = parameters.treble * 1000;
    out.contour = parameters.contour * 1000;
    out.volume = parameters.volume * 1000;
    out.noise_gate_threshold = parameters.noise_gate_threshold * 1000;
    out.noise_gate_hysteresis = parameters.noise_gate_hysteresis * 1000;
    out.noise_gate_attack = parameters.noise_gate_attack * 1000;
    out.noise_gate_hold = parameters.noise_gate_hold * 1000;
    out.noise_gate_release = parameters.noise_gate_release * 1000;
    out.noise_gate_bypass = parameters.noise_gate_bypass * 1000;
    out.chorus_rate = parameters.chorus_rate * 1000;
    out.chorus_depth = parameters.chorus_depth * 1000;
    out.chorus_mix = parameters.chorus_mix * 1000;
    out.chorus_bypass = parameters.chorus_bypass * 1000;
    out.wah_position = parameters.wah_position * 1000;
    out.wah_vocal = parameters.wah_vocal * 1000;
    out.wah_bypass = parameters.wah_bypass * 1000;
    return 0;
}

template <>
std::optional<presets::ParametersData>
from_proto(const shrapnel_presets_PresetParameters &unpacked)
{
    return {
        {
            .amp_gain = unpacked.amp_gain / 1000.f,
            .amp_channel = unpacked.amp_channel / 1000.f,
            .bass = unpacked.bass / 1000.f,
            .middle = unpacked.middle / 1000.f,
            .treble = unpacked.treble / 1000.f,
            .contour = unpacked.contour / 1000.f,
            .volume = unpacked.volume / 1000.f,
            .noise_gate_threshold = unpacked.noise_gate_threshold / 1000.f,
            .noise_gate_hysteresis = unpacked.noise_gate_hysteresis / 1000.f,
            .noise_gate_attack = unpacked.noise_gate_attack / 1000.f,
            .noise_gate_hold = unpacked.noise_gate_hold / 1000.f,
            .noise_gate_release = unpacked.noise_gate_release / 1000.f,
            .noise_gate_bypass = unpacked.noise_gate_bypass / 1000.f,
            .chorus_rate = unpacked.chorus_rate / 1000.f,
            .chorus_depth = unpacked.chorus_depth / 1000.f,
            .chorus_mix = unpacked.chorus_mix / 1000.f,
            .chorus_bypass = unpacked.chorus_bypass / 1000.f,
            .wah_position = unpacked.wah_position / 1000.f,
            .wah_vocal = unpacked.wah_vocal / 1000.f,
            .wah_bypass = unpacked.wah_bypass / 1000.f,
        },
    };
}

template <>
int to_proto(const presets::PresetData &preset, shrapnel_presets_Preset &out)
{
    shrapnel_presets_Preset preset_proto = shrapnel_presets_Preset_init_zero;

    int rc = to_proto<shrapnel_presets_PresetParameters>(
        preset.parameters, preset_proto.parameters);
    if(rc != 0)
    {
        return -1;
    }

    strncpy(preset_proto.name, preset.name.data(), sizeof preset_proto.name);

    return 0;
}

template <>
std::optional<presets::PresetData>
from_proto(const shrapnel_presets_Preset &unpacked)
{
    auto parameters = from_proto<presets::ParametersData>(unpacked.parameters);
    if(!parameters.has_value())
    {
        return std::nullopt;
    }

    return {
        {
            .name{unpacked.name},
            .parameters{*parameters},
        },
    };
}

template <>
int to_proto(const presets::Initialise &, shrapnel_presets_Initialise &)
{
    return 0;
}

template <>
std::optional<presets::Initialise>
from_proto(const shrapnel_presets_Initialise &message)
{
    return {{}};
}

template <>
int to_proto(const presets::Notify &message, shrapnel_presets_Notify &out)
{
    out.preset.id = message.id;
    return to_proto<shrapnel_presets_Preset>(message.preset, out.preset.preset);
}

template <>
std::optional<presets::Notify>
from_proto(const shrapnel_presets_Notify &message)
{
    presets::Notify out{};
    out.id = message.preset.id;
    out.preset = *from_proto<presets::PresetData>(message.preset.preset);
    return out;
}

template <>
int to_proto(const presets::Create &message, shrapnel_presets_Create &out)
{
    return to_proto<shrapnel_presets_Preset>(message.preset, out.preset);
}

template <>
std::optional<presets::Create>
from_proto(const shrapnel_presets_Create &message)
{
    presets::Create out{};
    out.preset = *from_proto<presets::PresetData>(message.preset);
    return out;
}

template <>
int to_proto(const presets::Update &message, shrapnel_presets_Update &out)
{
    out.preset.id = message.id;
    return to_proto<shrapnel_presets_Preset>(message.preset, out.preset.preset);
}

template <>
std::optional<presets::Update>
from_proto(const shrapnel_presets_Update &message)
{
    presets::Update out{};
    out.id = message.preset.id;
    out.preset = *from_proto<presets::PresetData>(message.preset.preset);
    return out;
}

template <>
int to_proto(const presets::Delete &message, shrapnel_presets_Remove &out)
{
    out.id = message.id;
    return 0;
}

template <>
std::optional<presets::Delete>
from_proto(const shrapnel_presets_Remove &message)
{
    return {{.id{message.id}}};
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
std::optional<presets::PresetsApiMessage>
from_proto(const shrapnel_presets_Message &message)
{
    switch(message.which_message)
    {
    case shrapnel_presets_Message_initialise_tag:
        return from_proto<presets::Initialise>(message.message.initialise);
    case shrapnel_presets_Message_notify_tag:
        return from_proto<presets::Notify>(message.message.notify);
    case shrapnel_presets_Message_create_tag:
        return from_proto<presets::Create>(message.message.create);
    case shrapnel_presets_Message_update_tag:
        return from_proto<presets::Update>(message.message.update);
    case shrapnel_presets_Message_remove_tag:
        return from_proto<presets::Delete>(message.message.remove);
    }

    return std::nullopt;
}

} // namespace shrapnel::api