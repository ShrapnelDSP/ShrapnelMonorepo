#pragma once

#include "presets.pb-c.h"
#include <esp_log.h>

template <typename T>
void serialise_parameters(T &parameters)
{
    struct PresetParameters preset = PRESET_PARAMETERS__INIT;
    preset_parameters__init(&preset);

    preset.amp_gain = parameters.get("ampGain") * 1000;
    preset.amp_channel = parameters.get("ampChannel") * 1000;
    preset.bass = parameters.get("bass") * 1000;
    preset.middle = parameters.get("middle") * 1000;
    preset.treble = parameters.get("treble") * 1000;
    preset.contour = parameters.get("contour") * 1000;
    preset.volume = parameters.get("volume") * 1000;
    preset.noise_gate_threshold = parameters.get("noiseGateThreshold") * 1000;
    preset.noise_gate_hysteresis = parameters.get("noiseGateHysteresis") * 1000;
    preset.noise_gate_attack = parameters.get("noiseGateAttack") * 1000;
    preset.noise_gate_hold = parameters.get("noiseGateHold") * 1000;
    preset.noise_gate_release = parameters.get("noiseGateRelease") * 1000;
    preset.noise_gate_bypass = parameters.get("noiseGateBypass") * 1000;
    preset.chorus_rate = parameters.get("chorusRate") * 1000;
    preset.chorus_depth = parameters.get("chorusDepth") * 1000;
    preset.chorus_mix = parameters.get("chorusMix") * 1000;
    preset.chorus_bypass = parameters.get("chorusBypass") * 1000;
    preset.wah_position = parameters.get("wahPosition") * 1000;
    preset.wah_vocal = parameters.get("wahVocal") * 1000;
    preset.wah_bypass = parameters.get("wahBypass") * 1000;

    uint8_t buffer[100];
    preset_parameters__pack(&preset, buffer);

    ESP_LOG_BUFFER_HEX("presets", buffer, preset_parameters__get_packed_size(&preset));
    ESP_LOGI("presets", "Packed size: %d", preset_parameters__get_packed_size(&preset));
}
