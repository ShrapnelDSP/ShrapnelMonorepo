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

#include "selected_preset_json_builder.h"
#include "selected_preset_message_type.h"

namespace shrapnel::selected_preset {

template <>
rapidjson::Value to_json(rapidjson::Document &document, const Notify &object)
{
    rapidjson::Value json;
    json.SetObject();

    json.AddMember<unsigned int>(
        "selectedPreset", object.selectedPresetId, document.GetAllocator());

    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const SelectedPresetApiMessage &object)
{
    rapidjson::Value json;

    auto visitor = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, Notify>)
        {
            json = to_json(document, message);
        }

        json.AddMember("messageType",
                       rapidjson::StringRef(get_message_type<T>()),
                       document.GetAllocator());
    };

    std::visit(visitor, object);

    return json;
}

} // namespace shrapnel::selected_preset