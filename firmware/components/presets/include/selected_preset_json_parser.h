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

#include "selected_preset_api.h"
#include <optional>

namespace shrapnel::selected_preset {

template <typename T>
std::optional<T> from_json(const rapidjson::Value &);

template <>
std::optional<Read> from_json(const rapidjson::Value &json);

template <>
std::optional<Write> from_json(const rapidjson::Value &json);

template <>
std::optional<SelectedPresetApiMessage> from_json(const rapidjson::Value &json);

} // namespace shrapnel::selected_preset