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

#include <cstdint>
#include <optional>
#include <span>

namespace shrapnel::api {

/// Convert a message to bytes to be sent over the network to the frontend
template <typename T>
std::optional<std::span<uint8_t>> to_bytes(const T &message,
                                           std::span<uint8_t> buffer);

/// Parse a message from bytes received from the frontend
template <typename T>
std::optional<T> from_bytes(std::span<const uint8_t> buffer);

/// Convert a message to proto struct
template <typename ProtoT, typename T>
[[nodiscard]] int to_proto(const T &message, ProtoT &out);

/// Convert a message from proto struct
template <typename T, typename ProtoT>
int from_proto(const ProtoT &message, T &out);

} // namespace shrapnel::api
