#pragma once

// Disable warning inside rapidjson
// https://github.com/Tencent/rapidjson/issues/1700
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include "rapidjson/document.h"
#pragma GCC diagnostic pop

#include "presets_api.h"
#include <optional>

namespace shrapnel::presets {

template <typename T>
std::optional<T> from_json(const rapidjson::Value &);

template <>
std::optional<ParametersData> from_json(const rapidjson::Value &json);

template <>
std::optional<PresetData> from_json(const rapidjson::Value &json);

template <>
std::optional<Initialise> from_json(const rapidjson::Value &json);

template <>
std::optional<Create> from_json(const rapidjson::Value &json);

template <>
std::optional<Update> from_json(const rapidjson::Value &json);

template <>
std::optional<PresetsApiMessage> from_json(const rapidjson::Value &json);

} // namespace shrapnel::selected_preset
