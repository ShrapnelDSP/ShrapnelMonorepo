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

/**
 * \page ws_api_events Audio event JSON messages
 *
 * This page documents the message format used for messages related to audio
 * events.
 *
 * <table>
 * <tr><th> Message type <th> Parameters <th> Direction <th> Behaviour <th>
 * Example
 * <tr>
 *   <td> `Event::input_clipped`
 *   <td> None
 *   <td> Firmware -> UI
 *   <td> Sent when the input level is too high, causing clipping
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "Event::input_clipped"
 *   }
 *   ~~~
 * <tr>
 *   <td> `Event::output_clipped`
 *   <td> None
 *   <td> Firmware -> UI
 *   <td> Sent when the output level is too high, causing clipping
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "Event::output_clipped"
 *   }
 *   ~~~
 * </table>
 */

#include "audio_events.h"

#define TAG "audio_events"

namespace {

template <typename T>
constexpr const char *get_message_type();

template <>
constexpr const char *get_message_type<shrapnel::events::InputClipped>()
{
    return "Event::input_clipped";
}

template <>
constexpr const char *get_message_type<shrapnel::events::OutputClipped>()
{
    return "Event::output_clipped";
}

} // namespace

namespace shrapnel::events {

std::atomic_flag input_clipped;
std::atomic_flag output_clipped;

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const ApiMessage &object)
{
    rapidjson::Value json;
    json.SetObject();

    auto visitor = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;
        json.AddMember("messageType",
                       rapidjson::StringRef(get_message_type<T>()),
                       document.GetAllocator());
    };

    std::visit(visitor, object);

    return json;
}

} // namespace shrapnel::events