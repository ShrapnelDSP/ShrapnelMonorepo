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
#include "audio_events.pb.h"

#define TAG "audio_events"

namespace shrapnel::events {

std::atomic_flag input_clipped;
std::atomic_flag output_clipped;

} // namespace shrapnel::events

namespace shrapnel::api {

template <>
std::optional<shrapnel_audio_events_InputClipped>
to_proto(const events::InputClipped &message)
{
    return {{}};
}

template <>
std::optional<events::InputClipped>
from_proto(const shrapnel_audio_events_InputClipped &message)
{
    return {{}};
}

template <>
std::optional<shrapnel_audio_events_OutputClipped>
to_proto(const events::OutputClipped &message)
{
    return {{}};
}

template <>
std::optional<events::OutputClipped>
from_proto(const shrapnel_audio_events_OutputClipped &message)
{
    return {{}};
}

template <>
std::optional<shrapnel_audio_events_Message>
to_proto(const events::ApiMessage &message)
{
    return std::visit(
        [](const auto &message) -> std::optional<shrapnel_audio_events_Message>
        {
            shrapnel_audio_events_Message out =
                shrapnel_audio_events_Message_init_zero;

            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, events::InputClipped>)
            {
                out.which_event =
                    shrapnel_audio_events_Message_input_clipped_tag;
                out.event.input_clipped =
                    *to_proto<shrapnel_audio_events_InputClipped>(message);
            }
            else if constexpr(std::is_same_v<T, events::OutputClipped>)
            {
                out.which_event =
                    shrapnel_audio_events_Message_output_clipped_tag;
                out.event.output_clipped =
                    *to_proto<shrapnel_audio_events_OutputClipped>(message);
            }
            else
            {
                return std::nullopt;
            }

            return out;
        },
        message);
}

template <>
std::optional<events::ApiMessage>
from_proto(const shrapnel_audio_events_Message &message)
{
    switch(message.which_event)
    {
    case shrapnel_audio_events_Message_input_clipped_tag:
        return from_proto<events::InputClipped>(message.event.input_clipped);
    case shrapnel_audio_events_Message_output_clipped_tag:
        return from_proto<events::OutputClipped>(message.event.output_clipped);
    }

    return std::nullopt;
}

} // namespace shrapnel::api