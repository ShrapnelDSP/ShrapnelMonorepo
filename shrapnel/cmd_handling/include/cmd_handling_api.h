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
#include "cmd_handling.pb.h"
#include <etl/string_stream.h>
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

etl::string_stream &operator<<(etl::string_stream &out, const Update &self);
etl::string_stream &operator<<(etl::string_stream &out, const Initialise &self);
etl::string_stream &operator<<(etl::string_stream &out, const ApiMessage &self);

} // namespace shrapnel::parameters

namespace shrapnel::api {

template <>
int to_proto(const parameters::ApiMessage &message,
             shrapnel_cmd_handling_Message &out);

template <>
int from_proto(const shrapnel_cmd_handling_Message &message,
               parameters::ApiMessage &out);

} // namespace shrapnel::api
