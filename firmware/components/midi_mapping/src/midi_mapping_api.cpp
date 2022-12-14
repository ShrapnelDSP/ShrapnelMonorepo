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

#include "midi_mapping_api.h"
#include "etl_utility.h"


namespace shrapnel {
namespace midi {

etl::string_stream& operator<<(etl::string_stream&  out, const Mapping& self) {
    out << "{";
    out << " channel " << self.midi_channel;
    out << " cc number " << self.cc_number;
    out << " mode "
        << (self.mode == Mapping::Mode::TOGGLE ? "toggle" : "parameter");
    out << " name " << self.parameter_name;
    out << " }";
    return out;
}

etl::string_stream& operator<<(etl::string_stream&  out, const GetRequest&) {
    out << "{}";
    return out;
}

etl::string_stream& operator<<(etl::string_stream&  out, const GetResponse& self)
{
    return out << "{ TODO }";
}

etl::string_stream& operator<<(etl::string_stream&  out, const CreateRequest& self) {
    out << "{ " << self.mapping << " }";
    return out;
}

etl::string_stream& operator<<(etl::string_stream&  out, const CreateResponse& self)
{
    return out << "{ " << self.mapping << " }";
}

etl::string_stream& operator<<(etl::string_stream&  out, const Update& self)
{
    return out << "{ " << self.mapping << " }";
}

etl::string_stream& operator<<(etl::string_stream&  out, const Remove& self)
{
    return out << "{ " << self.id << " }";
}

etl::string_stream& operator<<(etl::string_stream&  out, const MappingApiMessage& self)
{
#if 1
    auto print = [&](const auto &message) {
        using T = std::decay_t<decltype(message)>;

        if constexpr (std::is_same_v<T, GetRequest>) {
            out << "<GetRequest>" << message;
        } else if constexpr (std::is_same_v<T, GetResponse>) {
            out << "<GetResponse>" << message;
        } else if constexpr (std::is_same_v<T, CreateRequest>) {
            out << "<CreateRequest>" << message;
        } else if constexpr (std::is_same_v<T, CreateResponse>) {
            out << "<CreateResponse>" << message;
        } else if constexpr (std::is_same_v<T, Update>) {
            out << "<Update>" << message;
        } else if constexpr (std::is_same_v<T, Remove>) {
            out << "<Remove>" << message;
        } else {
            out << "Unknown";
        }
    };

    std::visit(print, self);

#else

    if (auto message = std::get_if<GetRequest>(&self)) {
        out << "<GetRequest>" << *message;
    } else if (auto message = std::get_if<GetResponse>(&self)) {
        out << "<GetResponse>" << *message;
    } else if (auto message = std::get_if<CreateRequest>(&self)) {
        out << "<CreateRequest>" << *message;
    } else if (auto message = std::get_if<CreateResponse>(&self)) {
        out << "<CreateResponse>" << *message;
    } else if (auto message = std::get_if<Update>(&self)) {
        out << "<Update>" << *message;
    } else if (auto message = std::get_if<Remove>(&self)) {
        out << "<Remove>" << *message;
    } else {
        out << "Unknown";
    }

#endif

    return out;
}

}
}
