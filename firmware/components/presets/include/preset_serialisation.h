#pragma once

#include "presets.pb-c.h"
#include <esp_log.h>

template <typename T>
void serialise_parameters(T &parameters)
{
    struct PresetParameters preset = PRESET_PARAMETERS__INIT;
    preset_parameters__init(&preset);

    struct Google__Protobuf__Int32Value amp_gain =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    amp_gain.value = parameters.get("ampGain") * 1000;

    struct Google__Protobuf__Int32Value amp_channel =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    amp_channel.value = parameters.get("ampChannel") * 1000;

    struct Google__Protobuf__Int32Value bass =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    bass.value = parameters.get("bass") * 1000;

    struct Google__Protobuf__Int32Value middle =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    middle.value = parameters.get("middle") * 1000;

    struct Google__Protobuf__Int32Value treble =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    treble.value = parameters.get("treble") * 1000;

    struct Google__Protobuf__Int32Value contour =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    contour.value = parameters.get("contour") * 1000;

    struct Google__Protobuf__Int32Value volume =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    volume.value = parameters.get("volume") * 1000;

    struct Google__Protobuf__Int32Value noise_gate_threshold =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    noise_gate_threshold.value = parameters.get("noiseGateThreshold") * 1000;

    struct Google__Protobuf__Int32Value noise_gate_hysteresis =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    noise_gate_hysteresis.value = parameters.get("noiseGateHysteresis") * 1000;

    struct Google__Protobuf__Int32Value noise_gate_attack =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    noise_gate_attack.value = parameters.get("noiseGateAttack") * 1000;

    struct Google__Protobuf__Int32Value noise_gate_hold =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    noise_gate_hold.value = parameters.get("noiseGateHold") * 1000;

    struct Google__Protobuf__Int32Value noise_gate_release =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    noise_gate_release.value = parameters.get("noiseGateRelease") * 1000;

    struct Google__Protobuf__Int32Value noise_gate_bypass =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    noise_gate_bypass.value = parameters.get("noiseGateBypass") * 1000;

    struct Google__Protobuf__Int32Value chorus_rate =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    chorus_rate.value = parameters.get("chorusRate") * 1000;

    struct Google__Protobuf__Int32Value chorus_depth =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    chorus_depth.value = parameters.get("chorusDepth") * 1000;

    struct Google__Protobuf__Int32Value chorus_mix =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    chorus_mix.value = parameters.get("chorusMix") * 1000;

    struct Google__Protobuf__Int32Value chorus_bypass =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    chorus_bypass.value = parameters.get("chorusBypass") * 1000;

    struct Google__Protobuf__Int32Value wah_position =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    wah_position.value = parameters.get("wahPosition") * 1000;

    struct Google__Protobuf__Int32Value wah_vocal =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    wah_vocal.value = parameters.get("wahVocal") * 1000;

    struct Google__Protobuf__Int32Value wah_bypass =
        GOOGLE__PROTOBUF__INT32_VALUE__INIT;
    wah_bypass.value = parameters.get("wahBypass") * 1000;

    preset.amp_gain = &amp_gain;
    preset.amp_channel = &amp_channel;
    preset.bass = &bass;
    preset.middle = &middle;
    preset.treble = &treble;
    preset.contour = &contour;
    preset.volume = &volume;
    preset.noise_gate_threshold = &noise_gate_threshold;
    preset.noise_gate_hysteresis = &noise_gate_hysteresis;
    preset.noise_gate_attack = &noise_gate_attack;
    preset.noise_gate_hold = &noise_gate_hold;
    preset.noise_gate_release = &noise_gate_release;
    preset.noise_gate_bypass = &noise_gate_bypass;
    preset.chorus_rate = &chorus_rate;
    preset.chorus_depth = &chorus_depth;
    preset.chorus_mix = &chorus_mix;
    preset.chorus_bypass = &chorus_bypass;
    preset.wah_position = &wah_position;
    preset.wah_vocal = &wah_vocal;
    preset.wah_bypass = &wah_bypass;

    uint8_t buffer[1000];
    preset_parameters__pack(&preset, buffer);

    ESP_LOG_BUFFER_HEX(
        "presets", buffer, preset_parameters__get_packed_size(&preset));
    ESP_LOGI("presets",
             "Packed size: %d",
             preset_parameters__get_packed_size(&preset));
}
