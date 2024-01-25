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

#include "presets_message_type.h"

namespace shrapnel::presets {

template <typename T>
constexpr const char *get_message_type();

template <>
constexpr const char *get_message_type<Initialise>()
{
    return "Presets::initialise";
}

template <>
constexpr const char *get_message_type<Create>()
{
    return "Presets::create";
}

template <>
constexpr const char *get_message_type<Notify>()
{
    return "Presets::notify";
}

template <>
constexpr const char *get_message_type<Update>()
{
    return "Presets::update";
}

template <>
constexpr const char *get_message_type<Delete>()
{
    return "Presets::delete";
}

} // namespace shrapnel::presets
