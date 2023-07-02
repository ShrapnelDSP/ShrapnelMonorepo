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

#include <audio_events.h>
#include <cmd_handling_api.h>
#include <midi_mapping_api.h>
#include <optional>
#include <variant>

using ApiMessage = std::variant<shrapnel::parameters::ApiMessage,
                                shrapnel::midi::MappingApiMessage,
                                shrapnel::events::ApiMessage>;
using FileDescriptor = std::optional<int>;
using AppMessage = std::pair<ApiMessage, FileDescriptor>;
