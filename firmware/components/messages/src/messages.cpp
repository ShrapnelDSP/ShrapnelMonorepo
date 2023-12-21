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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

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
int to_proto(const ApiMessage &message, shrapnel_messages_Message &out)
{
    ESP_LOGE("DEBUG",
             "%s stack %d",
             __FUNCTION__,
             uxTaskGetStackHighWaterMark(nullptr));

    return std::visit(
        [&out](const auto &message) -> int
        {
            using T = std::decay_t<decltype(message)>;
            ESP_LOGE("DEBUG",
                     "%s stack %d",
                     __FUNCTION__,
                     uxTaskGetStackHighWaterMark(nullptr));

            if constexpr(std::is_same_v<T, events::ApiMessage>)
            {
                out.which_message = shrapnel_messages_Message_audio_event_tag;
                return to_proto<shrapnel_audio_events_Message>(
                    message, out.message.audio_event);
            }
            else if constexpr(std::is_same_v<T, parameters::ApiMessage>)
            {
                out.which_message = shrapnel_messages_Message_command_tag;
                return to_proto<shrapnel_cmd_handling_Message>(
                    message, out.message.command);
            }
            else if constexpr(std::is_same_v<T, midi::MappingApiMessage>)
            {
                out.which_message = shrapnel_messages_Message_midi_mapping_tag;
                return to_proto<shrapnel_midi_mapping_Message>(
                    message, out.message.midi_mapping);
            }
            else if constexpr(std::is_same_v<T, presets::PresetsApiMessage>)
            {
                out.which_message = shrapnel_messages_Message_preset_tag;
                return to_proto<shrapnel_presets_Message>(message,
                                                          out.message.preset);
            }
            else if constexpr(std::is_same_v<
                                  T,
                                  selected_preset::SelectedPresetApiMessage>)
            {
                out.which_message =
                    shrapnel_messages_Message_selected_preset_tag;
                return to_proto<shrapnel_selected_preset_Message>(
                    message, out.message.selected_preset);
            }
            else
            {
                return -1;
            }
        },
        message);
}

template <>
int from_proto(const shrapnel_messages_Message &message, ApiMessage &out)
{
    switch(message.which_message)
    {
    case shrapnel_messages_Message_audio_event_tag:
    {
        events::ApiMessage tmp{};
        from_proto<events::ApiMessage>(message.message.audio_event, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_messages_Message_command_tag:
    {
        parameters::ApiMessage tmp{};
        from_proto<parameters::ApiMessage>(message.message.command, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_messages_Message_midi_mapping_tag:
    {
        midi::MappingApiMessage tmp{};
        from_proto<midi::MappingApiMessage>(message.message.midi_mapping, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_messages_Message_preset_tag:
    {
        presets::PresetsApiMessage tmp{};
        from_proto<presets::PresetsApiMessage>(message.message.preset, tmp);
        out = tmp;
        return 0;
    }
    case shrapnel_messages_Message_selected_preset_tag:
    {
        selected_preset::SelectedPresetApiMessage tmp{};
        from_proto<selected_preset::SelectedPresetApiMessage>(
            message.message.selected_preset, tmp);
        out = tmp;
        return 0;
    }
    }

    return -1;
}

template <>
std::optional<std::span<uint8_t>> to_bytes(const ApiMessage &message,
                                           std::span<uint8_t> buffer)
{
    ESP_LOGE("DEBUG",
             "%s before to_proto stack %d",
             __FUNCTION__,
             uxTaskGetStackHighWaterMark(nullptr));

    shrapnel_messages_Message proto_message =
        shrapnel_messages_Message_init_zero;
    int rc = to_proto<shrapnel_messages_Message>(message, proto_message);
    if(rc != 0)
    {
        return std::nullopt;
    }

    ESP_LOGE("DEBUG",
             "%s before to_bytes stack %d",
             __FUNCTION__,
             uxTaskGetStackHighWaterMark(nullptr));

    return to_bytes(proto_message, buffer);
}

template <>
std::optional<ApiMessage> from_bytes(std::span<const uint8_t> buffer)
{
    auto proto_message = from_bytes<shrapnel_messages_Message>(buffer);
    ApiMessage out{};
    int rc = from_proto<ApiMessage>(*proto_message, out);
    if(rc != 0)
    {
        return std::nullopt;
    }
    return out;
}

} // namespace shrapnel::api
