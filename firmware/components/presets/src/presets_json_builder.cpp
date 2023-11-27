#include "presets_json_builder.h"
#include "presets.pb-c.h"
#include "uuid.h"
#include <cppcodec/base64_default_rfc4648.hpp>
#include <esp_log.h>

namespace shrapnel::presets {

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const ParametersData &object)
{
    struct PresetParameters preset = PRESET_PARAMETERS__INIT;
    preset_parameters__init(&preset);

    preset.amp_gain = object.amp_gain * 1000;
    preset.amp_channel = object.amp_channel * 1000;
    preset.bass = object.bass * 1000;
    preset.middle = object.middle * 1000;
    preset.treble = object.treble * 1000;
    preset.contour = object.contour * 1000;
    preset.volume = object.volume * 1000;
    preset.noise_gate_threshold = object.noise_gate_threshold * 1000;
    preset.noise_gate_hysteresis = object.noise_gate_hysteresis * 1000;
    preset.noise_gate_attack = object.noise_gate_attack * 1000;
    preset.noise_gate_hold = object.noise_gate_hold * 1000;
    preset.noise_gate_release = object.noise_gate_release * 1000;
    preset.noise_gate_bypass = object.noise_gate_bypass * 1000;
    preset.chorus_rate = object.chorus_rate * 1000;
    preset.chorus_depth = object.chorus_depth * 1000;
    preset.chorus_mix = object.chorus_mix * 1000;
    preset.chorus_bypass = object.chorus_bypass * 1000;
    preset.wah_position = object.wah_position * 1000;
    preset.wah_vocal = object.wah_vocal * 1000;
    preset.wah_bypass = object.wah_bypass * 1000;

    uint8_t buffer[100] = {};
    auto packed_size = preset_parameters__get_packed_size(&preset);
    if(packed_size > 100)
    {
        return {/* null */};
    }

    preset_parameters__pack(&preset, buffer);

    ESP_LOG_BUFFER_HEX("presets", buffer, packed_size);
    ESP_LOGI("presets", "Packed size: %zu", packed_size);

    using base64_codec = cppcodec::base64_rfc4648;

    std::array<char, 200> base64_buffer;
    auto encoded_size = base64_codec::encoded_size(packed_size);
    if(encoded_size > base64_buffer.size())
    {
        return {/* null */};
    }

    base64_codec::encode(
        base64_buffer.data(), base64_buffer.size(), buffer, packed_size);

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

    json.AddMember(
        "id", uuid::to_json(document, object.id), document.GetAllocator());
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

    json.AddMember("messageType",
                   rapidjson::StringRef("Presets::notify"),
                   document.GetAllocator());

    json.AddMember(
        "preset", to_json(document, object.preset), document.GetAllocator());

    return json;
}

} // namespace shrapnel::presets
