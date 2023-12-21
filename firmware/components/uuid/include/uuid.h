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

#include "api.h"
#include "uuid.pb.h"
#include <array>
#include <cstdint>
#include <optional>

namespace shrapnel::uuid {
using uuid_t = std::array<uint8_t, 16>;
} // namespace shrapnel::uuid

namespace shrapnel::api {

template <>
int to_proto(const uuid::uuid_t &message, shrapnel_uuid_Uuid &out);

template <>
int
from_proto(const shrapnel_uuid_Uuid &message, uuid::uuid_t &out);

} // namespace shrapnel::api