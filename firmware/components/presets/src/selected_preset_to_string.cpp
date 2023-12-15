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
#include "selected_preset_api.h"

namespace shrapnel::selected_preset {

etl::string_stream &operator<<(etl::string_stream &out, const Read &self)
{
    return out << "{ }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Notify &self)
{
    return out << "{ selected_preset=" << self.selectedPresetId << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Write &self)
{
    return out << "{ selected_preset=" << self.selectedPresetId << " }";
}

etl::string_stream &operator<<(etl::string_stream &out,
                               const SelectedPresetApiMessage &self)
{
    auto print = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, Read>)
        {
            out << "<Read>" << message;
        }
        else if constexpr(std::is_same_v<T, Notify>)
        {
            out << "<Notify>" << message;
        }
        else if constexpr(std::is_same_v<T, Write>)
        {
            out << "<Write>" << message;
        }
        else
        {
            out << "Unknown";
        }
    };

    std::visit(print, self);
    return out;
}

} // namespace shrapnel::selected_preset