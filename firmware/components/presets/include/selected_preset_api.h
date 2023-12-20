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

#include <etl/string_stream.h>
#include <variant>

#include "api.h"
#include "presets.h"
#include "selected_preset.pb.h"
#include "uuid.h"

namespace shrapnel::selected_preset {

struct Read
{
    std::strong_ordering operator<=>(const Read &other) const = default;
};

struct Notify
{
    presets::id_t selectedPresetId;

    std::strong_ordering operator<=>(const Notify &other) const = default;
};

struct Write
{
    presets::id_t selectedPresetId;

    std::strong_ordering operator<=>(const Write &other) const = default;
};

using SelectedPresetApiMessage = std::variant<Read, Notify, Write>;

etl::string_stream &operator<<(etl::string_stream &out, const Read &self);
etl::string_stream &operator<<(etl::string_stream &out, const Notify &self);
etl::string_stream &operator<<(etl::string_stream &out, const Write &self);
etl::string_stream &operator<<(etl::string_stream &out,
                               const SelectedPresetApiMessage &self);

} // namespace shrapnel::selected_preset

namespace shrapnel::api {

template <>
std::optional<shrapnel_selected_preset_Message> to_proto(const selected_preset::SelectedPresetApiMessage &message);

template <>
std::optional<selected_preset::SelectedPresetApiMessage> from_proto(const shrapnel_selected_preset_Message &message);

}