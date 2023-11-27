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

#include <array>
#include <cstdint>
#include <optional>

namespace shrapnel::uuid {
using uuid_t = std::array<uint8_t, 16>;

template <typename T>
std::optional<T> from_json(const rapidjson::Value &json);

template <>
std::optional<uuid_t> from_json(const rapidjson::Value &json);

template <typename T>
rapidjson::Value to_json(rapidjson::Document &document, const T &object);

template <>
rapidjson::Value to_json(rapidjson::Document &document, const uuid_t &object);

} // namespace shrapnel::uuid