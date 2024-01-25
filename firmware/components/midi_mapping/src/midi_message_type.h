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

#include "midi_mapping_api.h"

namespace shrapnel {
namespace midi {

template <typename T>
constexpr const char *get_message_type();

template <>
constexpr const char *get_message_type<GetRequest>()
{
    return "MidiMap::get::request";
}

template <>
constexpr const char *get_message_type<GetResponse>()
{
    return "MidiMap::get::response";
}

template <>
constexpr const char *get_message_type<CreateRequest>()
{
    return "MidiMap::create::request";
}

template <>
constexpr const char *get_message_type<CreateResponse>()
{
    return "MidiMap::create::response";
}

template <>
constexpr const char *get_message_type<Update>()
{
    return "MidiMap::update";
}

template <>
constexpr const char *get_message_type<Remove>()
{
    return "MidiMap::remove";
}

template <>
constexpr const char *get_message_type<MessageReceived>()
{
    return "MidiMap::midi_message_received";
}

} // namespace midi
} // namespace shrapnel
