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

#include <api.h>
#include <audio_events.h>
#include <cmd_handling_api.h>
#include <midi_mapping_api.h>
#include <optional>
#include <presets_api.h>
#include <selected_preset_api.h>
#include <variant>

using ApiMessage =
    std::variant<shrapnel::parameters::ApiMessage,
                 shrapnel::midi::MappingApiMessage,
                 shrapnel::events::ApiMessage,
                 shrapnel::selected_preset::SelectedPresetApiMessage,
                 shrapnel::presets::PresetsApiMessage,
                 shrapnel::midi::Message>;
using FileDescriptor = std::optional<int>;
using AppMessage = std::pair<ApiMessage, FileDescriptor>;

etl::string_stream &operator<<(etl::string_stream &out, const ApiMessage &self);

namespace shrapnel::api {

template <>
std::optional<std::span<uint8_t>> to_bytes(const ApiMessage &message,
                                           std::span<uint8_t> buffer);

template <>
std::optional<ApiMessage> from_bytes(std::span<const uint8_t> buffer);

} // namespace shrapnel::api