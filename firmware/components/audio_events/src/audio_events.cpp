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
int to_proto(const events::InputClipped &, shrapnel_audio_events_InputClipped &)
{
    return 0;
}

template <>
int from_proto(const shrapnel_audio_events_InputClipped &,
               events::InputClipped &)
{
    return 0;
}

template <>
int to_proto(const events::OutputClipped &,
             shrapnel_audio_events_OutputClipped &)
{
    return 0;
}

template <>
int from_proto(const shrapnel_audio_events_OutputClipped &,
               events::OutputClipped &)
{
    return 0;
}

template <>
int to_proto(const events::ApiMessage &message,
             shrapnel_audio_events_Message &out)
{
    return std::visit(
        [&out](const auto &message) -> int
        {
            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, events::InputClipped>)
            {
                out.which_event =
                    shrapnel_audio_events_Message_input_clipped_tag;
                int rc = to_proto<shrapnel_audio_events_InputClipped>(
                    message, out.event.input_clipped);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, events::OutputClipped>)
            {
                out.which_event =
                    shrapnel_audio_events_Message_output_clipped_tag;
                int rc = to_proto<shrapnel_audio_events_OutputClipped>(
                    message, out.event.output_clipped);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }

            return 0;
        },
        message);
}

template <>
int from_proto(const shrapnel_audio_events_Message &message,
               events::ApiMessage &out)
{
    switch(message.which_event)
    {
    case shrapnel_audio_events_Message_input_clipped_tag:
    {
        events::InputClipped tmp{};
        int rc =
            from_proto<events::InputClipped>(message.event.input_clipped, tmp);
        out = tmp;
        return rc;
    }
    case shrapnel_audio_events_Message_output_clipped_tag:
    {
        events::OutputClipped tmp{};
        int rc = from_proto<events::OutputClipped>(message.event.output_clipped,
                                                   tmp);
        out = tmp;
        return rc;
    }
    }

    return -1;
}

} // namespace shrapnel::api