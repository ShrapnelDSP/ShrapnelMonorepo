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

#pragma once

#include "rapidjson/document.h"
#include <atomic>
#include <variant>

namespace shrapnel::events {

template <typename T>
rapidjson::Value to_json(rapidjson::Document &document, const T &object);

struct InputClipped
{
    std::strong_ordering operator<=>(const InputClipped &other) const = default;
};
struct OutputClipped
{
    std::strong_ordering operator<=>(const OutputClipped &other) const = default;
};

using ApiMessage = std::variant<InputClipped, OutputClipped>;

extern std::atomic_flag input_clipped;
extern std::atomic_flag output_clipped;

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const ApiMessage &object);

} // namespace shrapnel::events
