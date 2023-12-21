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

#include "uuid.h"

namespace shrapnel::api {

template <>
int to_proto(const uuid::uuid_t &message, shrapnel_uuid_Uuid &out)
{
    std::copy(std::begin(message), std::end(message), std::begin(out.value));
    return 0;
}

template <>
int 
from_proto(const shrapnel_uuid_Uuid &message, uuid::uuid_t &out)
{
    out = std::to_array(message.value);
    return 0;
}

} // namespace shrapnel::api