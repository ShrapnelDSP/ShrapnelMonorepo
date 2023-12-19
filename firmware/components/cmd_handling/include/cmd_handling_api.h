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
#include "audio_param.h"
#include <variant>

namespace shrapnel::parameters {

struct Update final
{
    id_t id;
    float value;

    std::strong_ordering operator<=>(const Update &other) const = default;
};

struct Initialise final
{
    std::strong_ordering operator<=>(const Initialise &other) const = default;
};

using ApiMessage = std::variant<Update, Initialise>;

} // namespace shrapnel::parameters

namespace shrapnel::api {

std::optional<std::span<uint8_t>>
to_bytes(const parameters::ApiMessage &message, std::span<uint8_t> buffer);

template<>
std::optional<parameters::ApiMessage>
from_bytes(std::span<const uint8_t> buffer);

}
