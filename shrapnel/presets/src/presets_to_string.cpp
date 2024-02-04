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

#include "etl_utility.h"
#include "presets_api.h"

namespace shrapnel::presets {

etl::string_stream &operator<<(etl::string_stream &out,
                               const ParametersData &self)
{
    return out << "{ amp_gain=" << self.amp_gain << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const PresetData &self)
{
    return out << "{ name=" << self.name << "parameters=" << self.parameters
               << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Initialise &self)
{
    return out << "{ }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Notify &self)
{
    return out << "{ id=" << self.id << " preset=" << self.preset << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Create &self)
{
    return out << "{ preset=" << self.preset << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Update &self)
{
    return out << "{ id= " << self.id << " preset=" << self.preset << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Delete &self)
{
    return out << "{ id=" << self.id << " }";
}

etl::string_stream &operator<<(etl::string_stream &out,
                               const PresetsApiMessage &self)
{
    auto print = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, Initialise>)
        {
            out << "<Initialise>" << message;
        }
        else if constexpr(std::is_same_v<T, Notify>)
        {
            out << "<Notify>" << message;
        }
        else if constexpr(std::is_same_v<T, Create>)
        {
            out << "<Create>" << message;
        }
        else if constexpr(std::is_same_v<T, Update>)
        {
            out << "<Update>" << message;
        }
        else if constexpr(std::is_same_v<T, Delete>)
        {
            out << "<Delete>" << message;
        }
        else
        {
            out << "Unknown";
        }
    };

    std::visit(print, self);
    return out;
}

} // namespace shrapnel::presets