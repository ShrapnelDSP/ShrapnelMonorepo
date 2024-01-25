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

#include "selected_preset_api.h"

namespace shrapnel::selected_preset {

template <typename T>
constexpr const char *get_message_type();

template <>
constexpr const char *get_message_type<Read>()
{
    return "SelectedPreset::read";
}

template <>
constexpr const char *get_message_type<Notify>()
{
    return "SelectedPreset::notify";
}

template <>
constexpr const char *get_message_type<Write>()
{
    return "SelectedPreset::write";
}

} // namespace shrapnel::selected_preset
