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

#include "cmd_handling_api.h"
#include "cmd_handling.pb.h"

namespace shrapnel::parameters {

etl::string_stream &operator<<(etl::string_stream &out, const Update &self)
{
    return out << "{ id=" << self.id << " value=" << self.value << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Initialise &self)
{
    return out << "{ }";
}

etl::string_stream &operator<<(etl::string_stream &out, const ApiMessage &self)
{
    std::visit(
        [&](const auto &message)
        {
            using T = std::decay_t<decltype(message)>;

            if constexpr(std::is_same_v<T, Update>)
            {
                out << "<Update>" << message;
            }
            else if constexpr(std::is_same_v<T, Initialise>)
            {
                out << "<Initialise>" << message;
            }
            else
            {
                ESP_LOGE("DEBUG", "unknown ?!");
                out << "<Unknown>";
            }
        },
        self);

    return out;
}

} // namespace shrapnel::parameters

namespace shrapnel::api {

template <>
int to_proto(const parameters::Update &message,
             shrapnel_cmd_handling_Update &out)
{
    out.value = message.value;
    strncpy(out.id, message.id.data(), sizeof out.id);
    return 0;
}

template <>
int to_proto(const parameters::Initialise &, shrapnel_cmd_handling_Initialise &)
{
    return 0;
}

template <>
int to_proto(const parameters::ApiMessage &message,
             shrapnel_cmd_handling_Message &out)
{
    return std::visit(
        [&out](const auto &message) -> int
        {
            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, parameters::Update>)
            {
                out.which_message = shrapnel_cmd_handling_Message_update_tag;
                return to_proto<shrapnel_cmd_handling_Update>(
                    message, out.message.update);
            }
            else if constexpr(std::is_same_v<T, parameters::Initialise>)
            {
                out.which_message =
                    shrapnel_cmd_handling_Message_initialise_tag;
                return to_proto<shrapnel_cmd_handling_Initialise>(
                    message, out.message.initialise);
            }
            return -1;
        },
        message);
}

template <>
int from_proto(const shrapnel_cmd_handling_Update &message,
               parameters::Update &out)
{
    out.id = message.id;
    out.value = message.value;
    return 0;
}

template <>
int from_proto(const shrapnel_cmd_handling_Initialise &,
               parameters::Initialise &)
{
    return 0;
}

template <>
int from_proto(const shrapnel_cmd_handling_Message &message,
               parameters::ApiMessage &out)
{
    switch(message.which_message)
    {
    case shrapnel_cmd_handling_Message_update_tag:
    {
        parameters::Update tmp{};
        int rc = from_proto<parameters::Update>(message.message.update, tmp);
        out = tmp;
        return rc;
    }
    case shrapnel_cmd_handling_Message_initialise_tag:
    {
        parameters::Initialise tmp{};
        int rc =
            from_proto<parameters::Initialise>(message.message.initialise, tmp);
        out = tmp;
        return rc;
    }
    }

    return -1;
}

}; // namespace shrapnel::api