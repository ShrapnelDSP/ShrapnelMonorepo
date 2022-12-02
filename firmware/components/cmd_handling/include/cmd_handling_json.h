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

#include "cmd_handling_api.h"
#include "rapidjson/document.h"
#include <optional>

// TODO move the template JSON function declarations to a common JSON header

namespace shrapnel::parameters {

template<typename T>
std::optional<T> from_json(const rapidjson::Value &json);

template<>
std::optional<Update> from_json(const rapidjson::Value &json);

template<>
std::optional<Initialise> from_json(const rapidjson::Value &json);

template<>
std::optional<ApiMessage> from_json(const rapidjson::Value &json);


template<typename T>
constexpr const char *get_message_type();

template<>
constexpr const char *get_message_type<Update>() {
    return "parameterUpdate";
}

template<>
constexpr const char *get_message_type<Initialise>() {
    return "initialiseParameters";
}

}
