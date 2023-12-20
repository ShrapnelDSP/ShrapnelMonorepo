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

#include "messages.h"
#include "shrapnel.pb.h"
#include <pb_decode.h>
#include <pb_encode.h>

namespace shrapnel::api {

template <>
std::optional<std::span<uint8_t>>
to_bytes(const shrapnel_messages_Message &message, std::span<uint8_t> buffer)
{
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    bool success = pb_encode(&stream, &shrapnel_messages_Message_msg, &message);
    if(!success)
    {
        return std::nullopt;
    }

    return buffer.subspan(0, stream.bytes_written);
}

template <>
std::optional<shrapnel_messages_Message>
from_bytes(std::span<const uint8_t> buffer)
{
    auto stream = pb_istream_from_buffer(buffer.data(), buffer.size());
    shrapnel_messages_Message unpacked = shrapnel_messages_Message_init_zero;
    bool success =
        pb_decode(&stream, &shrapnel_messages_Message_msg, &unpacked);
    if(!success)
    {
        return std::nullopt;
    }

    return unpacked;
}

template <>
std::optional<shrapnel_messages_Message> to_proto(const ApiMessage &message)
{
    return std::visit(
        [&](const auto &message) -> std::optional<shrapnel_messages_Message>
        {
            using T = std::decay_t<decltype(message)>;

            shrapnel_messages_Message out = shrapnel_messages_Message_init_zero;

            if constexpr(std::is_same_v<T, events::ApiMessage>)
            {
                out.which_message = shrapnel_messages_Message_audio_event_tag;
                out.message.audio_event =
                    *to_proto<shrapnel_audio_events_Message>(message);
                return out;
            }
            else if constexpr(std::is_same_v<T, parameters::ApiMessage>)
            {
                out.which_message = shrapnel_messages_Message_command_tag;
                out.message.command =
                    *to_proto<shrapnel_cmd_handling_Message>(message);
                return out;
            }
            else if constexpr(std::is_same_v<T, midi::MappingApiMessage>)
            {
                out.which_message = shrapnel_messages_Message_midi_mapping_tag;
                out.message.midi_mapping =
                    *to_proto<shrapnel_midi_mapping_Message>(message);
                return out;
            }
            else if constexpr(std::is_same_v<T, presets::PresetsApiMessage>)
            {
                out.which_message = shrapnel_messages_Message_preset_tag;
                out.message.preset =
                    *to_proto<shrapnel_presets_Message>(message);
                return out;
            }
            else if constexpr(std::is_same_v<
                                  T,
                                  selected_preset::SelectedPresetApiMessage>)
            {
                out.which_message =
                    shrapnel_messages_Message_selected_preset_tag;
                out.message.selected_preset =
                    *to_proto<shrapnel_selected_preset_Message>(message);
                return out;
            }
            else
            {
                return std::nullopt;
            }
        },
        message);
}

template <>
std::optional<ApiMessage> from_proto(const shrapnel_messages_Message &message)
{
    switch(message.which_message)
    {
    case shrapnel_messages_Message_audio_event_tag:
        return from_proto<events::ApiMessage>(message.message.audio_event);
    case shrapnel_messages_Message_command_tag:
        return from_proto<parameters::ApiMessage>(message.message.command);
    case shrapnel_messages_Message_midi_mapping_tag:
        return from_proto<midi::MappingApiMessage>(
            message.message.midi_mapping);
    case shrapnel_messages_Message_preset_tag:
        return from_proto<presets::PresetsApiMessage>(message.message.preset);
    case shrapnel_messages_Message_selected_preset_tag:
        return from_proto<selected_preset::SelectedPresetApiMessage>(
            message.message.selected_preset);
    }

    return std::nullopt;
}

template <>
std::optional<std::span<uint8_t>> to_bytes(const ApiMessage &message,
                                           std::span<uint8_t> buffer)
{
    auto proto_message = to_proto<shrapnel_messages_Message>(message);
    return to_bytes(*proto_message, buffer);
}

template <>
std::optional<ApiMessage> from_bytes(std::span<const uint8_t> buffer)
{
    auto proto_message = from_bytes<shrapnel_messages_Message>(buffer);
    return from_proto<ApiMessage>(*proto_message);
}

} // namespace shrapnel::api
