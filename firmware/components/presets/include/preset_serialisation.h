#pragma once

#include "presets.pb-c.h"
#include "presets_api.h"
#include <esp_log.h>
#include <span>

namespace shrapnel::presets {

template <typename T>
ParametersData serialise_live_parameters(T &parameters_service)
{
    shrapnel::presets::ParametersData parameters{
        .amp_gain = parameters_service.get("ampGain"),
        .amp_channel = parameters_service.get("ampChannel"),
        .bass = parameters_service.get("bass"),
        .middle = parameters_service.get("middle"),
        .treble = parameters_service.get("treble"),
        .contour = parameters_service.get("contour"),
        .volume = parameters_service.get("volume"),
        .noise_gate_threshold = parameters_service.get("noiseGateThreshold"),
        .noise_gate_hysteresis = parameters_service.get("noiseGateHysteresis"),
        .noise_gate_attack = parameters_service.get("noiseGateAttack"),
        .noise_gate_hold = parameters_service.get("noiseGateHold"),
        .noise_gate_release = parameters_service.get("noiseGateRelease"),
        .noise_gate_bypass = parameters_service.get("noiseGateBypass"),
        .chorus_rate = parameters_service.get("chorusRate"),
        .chorus_depth = parameters_service.get("chorusDepth"),
        .chorus_mix = parameters_service.get("chorusMix"),
        .chorus_bypass = parameters_service.get("chorusBypass"),
        .wah_position = parameters_service.get("wahPosition"),
        .wah_vocal = parameters_service.get("wahVocal"),
        .wah_bypass = parameters_service.get("wahBypass"),
    };

    return parameters;
}

template <typename T>
void deserialise_live_parameters(T &parameters_service,
                                 shrapnel::presets::ParametersData parameters)
{
    parameters_service.update("ampGain", parameters.amp_gain);
    parameters_service.update("ampChannel", parameters.amp_channel);
    parameters_service.update("bass", parameters.bass);
    parameters_service.update("middle", parameters.middle);
    parameters_service.update("treble", parameters.treble);
    parameters_service.update("contour", parameters.contour);
    parameters_service.update("volume", parameters.volume);
    parameters_service.update("noiseGateThreshold", parameters.noise_gate_threshold);
    parameters_service.update("noiseGateHysteresis", parameters.noise_gate_hysteresis);
    parameters_service.update("noiseGateAttack", parameters.noise_gate_attack);
    parameters_service.update("noiseGateHold", parameters.noise_gate_hold);
    parameters_service.update("noiseGateRelease", parameters.noise_gate_release);
    parameters_service.update("noiseGateBypass", parameters.noise_gate_bypass);
    parameters_service.update("chorusRate", parameters.chorus_rate);
    parameters_service.update("chorusDepth", parameters.chorus_depth);
    parameters_service.update("chorusMix", parameters.chorus_mix);
    parameters_service.update("chorusBypass", parameters.chorus_bypass);
    parameters_service.update("wahPosition", parameters.wah_position);
    parameters_service.update("wahVocal", parameters.wah_vocal);
    parameters_service.update("wahBypass", parameters.wah_bypass);
}

inline int serialise_parameters(const ParametersData &parameters,
                                std::span<uint8_t> &buffer)
{
    struct PresetParameters preset = PRESET_PARAMETERS__INIT;
    preset_parameters__init(&preset);

    preset.amp_gain = parameters.amp_gain * 1000;
    preset.amp_channel = parameters.amp_channel * 1000;
    preset.bass = parameters.bass * 1000;
    preset.middle = parameters.middle * 1000;
    preset.treble = parameters.treble * 1000;
    preset.contour = parameters.contour * 1000;
    preset.volume = parameters.volume * 1000;
    preset.noise_gate_threshold = parameters.noise_gate_threshold * 1000;
    preset.noise_gate_hysteresis = parameters.noise_gate_hysteresis * 1000;
    preset.noise_gate_attack = parameters.noise_gate_attack * 1000;
    preset.noise_gate_hold = parameters.noise_gate_hold * 1000;
    preset.noise_gate_release = parameters.noise_gate_release * 1000;
    preset.noise_gate_bypass = parameters.noise_gate_bypass * 1000;
    preset.chorus_rate = parameters.chorus_rate * 1000;
    preset.chorus_depth = parameters.chorus_depth * 1000;
    preset.chorus_mix = parameters.chorus_mix * 1000;
    preset.chorus_bypass = parameters.chorus_bypass * 1000;
    preset.wah_position = parameters.wah_position * 1000;
    preset.wah_vocal = parameters.wah_vocal * 1000;
    preset.wah_bypass = parameters.wah_bypass * 1000;

    auto packed_size = preset_parameters__get_packed_size(&preset);
    if(packed_size > buffer.size())
    {
        return -1;
    }

    preset_parameters__pack(&preset, buffer.data());
    buffer = buffer.subspan(0, packed_size);
    return 0;
}

inline int serialise_preset(const PresetData &preset,
                            std::span<uint8_t> &buffer)
{
    struct PresetParameters parameters_proto = PRESET_PARAMETERS__INIT;
    preset_parameters__init(&parameters_proto);

    parameters_proto.amp_gain = preset.parameters.amp_gain * 1000;
    parameters_proto.amp_channel = preset.parameters.amp_channel * 1000;
    parameters_proto.bass = preset.parameters.bass * 1000;
    parameters_proto.middle = preset.parameters.middle * 1000;
    parameters_proto.treble = preset.parameters.treble * 1000;
    parameters_proto.contour = preset.parameters.contour * 1000;
    parameters_proto.volume = preset.parameters.volume * 1000;
    parameters_proto.noise_gate_threshold =
        preset.parameters.noise_gate_threshold * 1000;
    parameters_proto.noise_gate_hysteresis =
        preset.parameters.noise_gate_hysteresis * 1000;
    parameters_proto.noise_gate_attack =
        preset.parameters.noise_gate_attack * 1000;
    parameters_proto.noise_gate_hold = preset.parameters.noise_gate_hold * 1000;
    parameters_proto.noise_gate_release =
        preset.parameters.noise_gate_release * 1000;
    parameters_proto.noise_gate_bypass =
        preset.parameters.noise_gate_bypass * 1000;
    parameters_proto.chorus_rate = preset.parameters.chorus_rate * 1000;
    parameters_proto.chorus_depth = preset.parameters.chorus_depth * 1000;
    parameters_proto.chorus_mix = preset.parameters.chorus_mix * 1000;
    parameters_proto.chorus_bypass = preset.parameters.chorus_bypass * 1000;
    parameters_proto.wah_position = preset.parameters.wah_position * 1000;
    parameters_proto.wah_vocal = preset.parameters.wah_vocal * 1000;
    parameters_proto.wah_bypass = preset.parameters.wah_bypass * 1000;

    struct Preset preset_proto = PRESET__INIT;
    preset__init(&preset_proto);

    preset_proto.name = const_cast<char *>(preset.name.c_str());
    preset_proto.parameters = &parameters_proto;

    auto packed_size = preset__get_packed_size(&preset_proto);
    if(packed_size > buffer.size())
    {
        return -1;
    }

    preset__pack(&preset_proto, buffer.data());
    buffer = buffer.subspan(0, packed_size);
    return 0;
}

inline int deserialise_preset(const std::span<uint8_t> &buffer,
                              PresetData &preset)
{
    auto unpacked = preset__unpack(nullptr, buffer.size(), buffer.data());
    if(unpacked == nullptr)
    {
        return -1;
    }

    preset = PresetData{
        .name{unpacked->name},
        .parameters{
            .amp_gain = unpacked->parameters->amp_gain / 1000.f,
            .amp_channel = unpacked->parameters->amp_channel / 1000.f,
            .bass = unpacked->parameters->bass / 1000.f,
            .middle = unpacked->parameters->middle / 1000.f,
            .treble = unpacked->parameters->treble / 1000.f,
            .contour = unpacked->parameters->contour / 1000.f,
            .volume = unpacked->parameters->volume / 1000.f,
            .noise_gate_threshold =
                unpacked->parameters->noise_gate_threshold / 1000.f,
            .noise_gate_hysteresis =
                unpacked->parameters->noise_gate_hysteresis / 1000.f,
            .noise_gate_attack =
                unpacked->parameters->noise_gate_attack / 1000.f,
            .noise_gate_hold = unpacked->parameters->noise_gate_hold / 1000.f,
            .noise_gate_release =
                unpacked->parameters->noise_gate_release / 1000.f,
            .noise_gate_bypass =
                unpacked->parameters->noise_gate_bypass / 1000.f,
            .chorus_rate = unpacked->parameters->chorus_rate / 1000.f,
            .chorus_depth = unpacked->parameters->chorus_depth / 1000.f,
            .chorus_mix = unpacked->parameters->chorus_mix / 1000.f,
            .chorus_bypass = unpacked->parameters->chorus_bypass / 1000.f,
            .wah_position = unpacked->parameters->wah_position / 1000.f,
            .wah_vocal = unpacked->parameters->wah_vocal / 1000.f,
            .wah_bypass = unpacked->parameters->wah_bypass / 1000.f,
        },
    };

    preset__free_unpacked(unpacked, nullptr);
    return 0;
}

} // namespace shrapnel::presets