#include "presets_api.h"
#include "presets.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

namespace shrapnel::api {

std::optional<shrapnel_presets_Preset>
to_proto(const presets::PresetData &preset)
{
    shrapnel_presets_Preset preset_proto = shrapnel_presets_Preset_init_zero;

    preset_proto.parameters.amp_gain = preset.parameters.amp_gain * 1000;
    preset_proto.parameters.amp_channel = preset.parameters.amp_channel * 1000;
    preset_proto.parameters.bass = preset.parameters.bass * 1000;
    preset_proto.parameters.middle = preset.parameters.middle * 1000;
    preset_proto.parameters.treble = preset.parameters.treble * 1000;
    preset_proto.parameters.contour = preset.parameters.contour * 1000;
    preset_proto.parameters.volume = preset.parameters.volume * 1000;
    preset_proto.parameters.noise_gate_threshold =
        preset.parameters.noise_gate_threshold * 1000;
    preset_proto.parameters.noise_gate_hysteresis =
        preset.parameters.noise_gate_hysteresis * 1000;
    preset_proto.parameters.noise_gate_attack =
        preset.parameters.noise_gate_attack * 1000;
    preset_proto.parameters.noise_gate_hold =
        preset.parameters.noise_gate_hold * 1000;
    preset_proto.parameters.noise_gate_release =
        preset.parameters.noise_gate_release * 1000;
    preset_proto.parameters.noise_gate_bypass =
        preset.parameters.noise_gate_bypass * 1000;
    preset_proto.parameters.chorus_rate = preset.parameters.chorus_rate * 1000;
    preset_proto.parameters.chorus_depth =
        preset.parameters.chorus_depth * 1000;
    preset_proto.parameters.chorus_mix = preset.parameters.chorus_mix * 1000;
    preset_proto.parameters.chorus_bypass =
        preset.parameters.chorus_bypass * 1000;
    preset_proto.parameters.wah_position =
        preset.parameters.wah_position * 1000;
    preset_proto.parameters.wah_vocal = preset.parameters.wah_vocal * 1000;
    preset_proto.parameters.wah_bypass = preset.parameters.wah_bypass * 1000;

    strncpy(preset_proto.name, preset.name.data(), sizeof preset_proto.name);

    return preset_proto;
}

std::optional<presets::PresetData>
from_proto(const shrapnel_presets_Preset &unpacked)
{
    return {{
        .name{unpacked.name},
        .parameters{
            .amp_gain = unpacked.parameters.amp_gain / 1000.f,
            .amp_channel = unpacked.parameters.amp_channel / 1000.f,
            .bass = unpacked.parameters.bass / 1000.f,
            .middle = unpacked.parameters.middle / 1000.f,
            .treble = unpacked.parameters.treble / 1000.f,
            .contour = unpacked.parameters.contour / 1000.f,
            .volume = unpacked.parameters.volume / 1000.f,
            .noise_gate_threshold =
                unpacked.parameters.noise_gate_threshold / 1000.f,
            .noise_gate_hysteresis =
                unpacked.parameters.noise_gate_hysteresis / 1000.f,
            .noise_gate_attack = unpacked.parameters.noise_gate_attack / 1000.f,
            .noise_gate_hold = unpacked.parameters.noise_gate_hold / 1000.f,
            .noise_gate_release =
                unpacked.parameters.noise_gate_release / 1000.f,
            .noise_gate_bypass = unpacked.parameters.noise_gate_bypass / 1000.f,
            .chorus_rate = unpacked.parameters.chorus_rate / 1000.f,
            .chorus_depth = unpacked.parameters.chorus_depth / 1000.f,
            .chorus_mix = unpacked.parameters.chorus_mix / 1000.f,
            .chorus_bypass = unpacked.parameters.chorus_bypass / 1000.f,
            .wah_position = unpacked.parameters.wah_position / 1000.f,
            .wah_vocal = unpacked.parameters.wah_vocal / 1000.f,
            .wah_bypass = unpacked.parameters.wah_bypass / 1000.f,
        },
    }};
}

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
    bool success = pb_decode(&stream, &shrapnel_presets_Message_msg, &unpacked);
    if(!success)
    {
        return std::nullopt;
    }

    return unpacked;
}

} // namespace shrapnel::api