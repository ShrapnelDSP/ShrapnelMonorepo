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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "etl_utility.h"
#include "midi_mapping_api.h"
#include <pb_decode.h>
#include <pb_encode.h>

namespace shrapnel {
namespace midi {

etl::string_stream &operator<<(etl::string_stream &out, const Mapping &self)
{
    out << "{";
    out << " channel " << self.midi_channel;
    out << " cc number " << self.cc_number;
    out << " mode "
        << (self.mode == Mapping::Mode::TOGGLE ? "toggle" : "parameter");
    out << " name " << self.parameter_name;
    out << " }";
    return out;
}

etl::string_stream &operator<<(etl::string_stream &out, const GetRequest &)
{
    out << "{}";
    return out;
}

etl::string_stream &operator<<(etl::string_stream &out, const GetResponse &)
{
    return out << "{ TODO }";
}

etl::string_stream &operator<<(etl::string_stream &out,
                               const CreateRequest &self)
{
    out << "{ " << self.mapping << " }";
    return out;
}

etl::string_stream &operator<<(etl::string_stream &out,
                               const CreateResponse &self)
{
    return out << "{ " << self.mapping << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Update &self)
{
    return out << "{ " << self.mapping << " }";
}

etl::string_stream &operator<<(etl::string_stream &out, const Remove &self)
{
    return out << "{ " << self.id << " }";
}

etl::string_stream &operator<<(etl::string_stream &out,
                               const MessageReceived &self)
{
    return out << "{ " << self.message << " }";
}

etl::string_stream &operator<<(etl::string_stream &out,
                               const MappingApiMessage &self)
{
#if 1
    auto print = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, GetRequest>)
        {
            out << "<GetRequest>" << message;
        }
        else if constexpr(std::is_same_v<T, GetResponse>)
        {
            out << "<GetResponse>" << message;
        }
        else if constexpr(std::is_same_v<T, CreateRequest>)
        {
            out << "<CreateRequest>" << message;
        }
        else if constexpr(std::is_same_v<T, CreateResponse>)
        {
            out << "<CreateResponse>" << message;
        }
        else if constexpr(std::is_same_v<T, Update>)
        {
            out << "<Update>" << message;
        }
        else if constexpr(std::is_same_v<T, Remove>)
        {
            out << "<Remove>" << message;
        }
        else
        {
            out << "Unknown";
        }
    };

    std::visit(print, self);

#else

    if(auto message = std::get_if<GetRequest>(&self))
    {
        out << "<GetRequest>" << *message;
    }
    else if(auto message = std::get_if<GetResponse>(&self))
    {
        out << "<GetResponse>" << *message;
    }
    else if(auto message = std::get_if<CreateRequest>(&self))
    {
        out << "<CreateRequest>" << *message;
    }
    else if(auto message = std::get_if<CreateResponse>(&self))
    {
        out << "<CreateResponse>" << *message;
    }
    else if(auto message = std::get_if<Update>(&self))
    {
        out << "<Update>" << *message;
    }
    else if(auto message = std::get_if<Remove>(&self))
    {
        out << "<Remove>" << *message;
    }
    else
    {
        out << "Unknown";
    }

#endif

    return out;
}

} // namespace midi

namespace api {

template <>
std::optional<std::span<uint8_t>>
to_bytes(const shrapnel_midi_mapping_Message &message,
         std::span<uint8_t> buffer)
{
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    bool success =
        pb_encode(&stream, &shrapnel_midi_mapping_Message_msg, &message);
    if(!success)
    {
        return std::nullopt;
    }

    return buffer.subspan(0, stream.bytes_written);
}

template <>
std::optional<shrapnel_midi_mapping_Message>
from_bytes(std::span<const uint8_t> buffer)
{
    auto stream = pb_istream_from_buffer(buffer.data(), buffer.size());
    shrapnel_midi_mapping_Message message =
        shrapnel_midi_mapping_Message_init_zero;
    bool success =
        pb_decode(&stream, &shrapnel_midi_mapping_Message_msg, &message);
    if(!success)
    {
        return std::nullopt;
    }

    return message;
}

template <>
std::optional<std::span<uint8_t>>
to_bytes(const shrapnel_midi_mapping_MappingList &message,
         std::span<uint8_t> buffer)
{
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    bool success =
        pb_encode(&stream, &shrapnel_midi_mapping_MappingList_msg, &message);
    if(!success)
    {
        return std::nullopt;
    }

    return buffer.subspan(0, stream.bytes_written);
}

template <>
std::optional<shrapnel_midi_mapping_MappingList>
from_bytes(std::span<const uint8_t> buffer)
{
    auto stream = pb_istream_from_buffer(buffer.data(), buffer.size());
    shrapnel_midi_mapping_MappingList message =
        shrapnel_midi_mapping_MappingList_init_zero;
    bool success =
        pb_decode(&stream, &shrapnel_midi_mapping_MappingList_msg, &message);
    if(!success)
    {
        return std::nullopt;
    }

    return message;
}

template <>
int to_proto(const midi::Mapping &message, shrapnel_midi_mapping_Mapping &out)
{
    out.midi_channel = message.midi_channel;
    out.cc_number = message.cc_number;
    switch(message.mode)
    {
    case midi::Mapping::Mode::PARAMETER:
        out.mode = shrapnel_midi_mapping_Mapping_Mode_parameter;
        break;
    case midi::Mapping::Mode::TOGGLE:
        out.mode = shrapnel_midi_mapping_Mapping_Mode_toggle;
        break;
    }
    strncpy(out.parameterName,
            message.parameter_name.data(),
            sizeof out.parameterName);
    return 0;
}

template <>
std::optional<midi::Mapping>
from_proto(const shrapnel_midi_mapping_Mapping &message)
{
    midi::Mapping out{
        .midi_channel = static_cast<uint8_t>(message.midi_channel),
        .cc_number = static_cast<uint8_t>(message.cc_number),
        .parameter_name{message.parameterName},
    };
    switch(message.mode)
    {
    case shrapnel_midi_mapping_Mapping_Mode_parameter:
        out.mode = midi::Mapping::Mode::PARAMETER;
        break;
    case shrapnel_midi_mapping_Mapping_Mode_toggle:
        out.mode = midi::Mapping::Mode::TOGGLE;
        break;
    }
    return out;
}

template <>
int to_proto(const std::pair<midi::Mapping::id_t, midi::Mapping> &message,
             shrapnel_midi_mapping_MappingRecord &out)
{
    ESP_LOGE("DEBUG",
             "%s stack %d",
             __FUNCTION__,
             uxTaskGetStackHighWaterMark(nullptr));

    int rc =
        to_proto<shrapnel_midi_mapping_Mapping>(message.second, out.mapping);
    if(rc != 0)
    {
        return -1;
    }
    // FIXME: do we have to set the has_mapping etc? In proto3, there is no
    // presence, so it should not even exist?
    rc = to_proto<shrapnel_uuid_Uuid>(message.first, out.id);
    if(rc != 0)
    {
        return -1;
    }
    return 0;
}

template <>
std::optional<std::pair<midi::Mapping::id_t, midi::Mapping>>
from_proto(const shrapnel_midi_mapping_MappingRecord &message)
{
    std::pair<midi::Mapping::id_t, midi::Mapping> out;

    auto id = from_proto<midi::Mapping::id_t>(message.id);
    if(!id.has_value())
    {
        return std::nullopt;
    }
    out.first = *id;

    auto mapping = from_proto<midi::Mapping>(message.mapping);
    if(!mapping.has_value())
    {
        return std::nullopt;
    }
    out.second = *mapping;
    return out;
}

template <>
int to_proto(const etl::map<midi::Mapping::id_t, midi::Mapping, 10> &message,
             shrapnel_midi_mapping_MappingList &out)
{
    ESP_LOGE("DEBUG",
             "%s stack %d",
             __FUNCTION__,
             uxTaskGetStackHighWaterMark(nullptr));

    int i = 0;
    for(const auto &[key, value] : message)
    {
        if(i >= (sizeof(out.mappings) / sizeof(out.mappings[0])))
        {
            break;
        }

        int rc = to_proto<shrapnel_midi_mapping_MappingRecord>(
            std::pair{key, value}, out.mappings[i]);
        if(rc != 0)
        {
            return -1;
        }
        i++;
    }
    out.mappings_count = i;

    return 0;
}

template <>
std::optional<etl::map<midi::Mapping::id_t, midi::Mapping, 10>>
from_proto(const shrapnel_midi_mapping_MappingList &message)
{
    etl::map<midi::Mapping::id_t, midi::Mapping, 10> out;

    for(size_t i = 0; i < message.mappings_count; i++)
    {
        auto &proto_mapping = message.mappings[i];
        auto id = from_proto<midi::Mapping::id_t>(proto_mapping.id);
        if(!id.has_value())
        {
            return std::nullopt;
        }
        auto mapping = api::from_proto<midi::Mapping>(proto_mapping.mapping);
        if(mapping.has_value())
        {
            out[*id] = *mapping;
        }
    }

    return out;
}

template <>
int to_proto(const midi::GetRequest &, shrapnel_midi_mapping_GetRequest &)
{
    return 0;
}

template <>
std::optional<midi::GetRequest>
from_proto(const shrapnel_midi_mapping_GetRequest &)
{
    midi::GetRequest out{};
    return out;
}

template <>
int to_proto(const midi::GetResponse &message,
             shrapnel_midi_mapping_GetResponse &out)
{
    ESP_LOGE("DEBUG",
             "%s stack %d",
             __FUNCTION__,
             uxTaskGetStackHighWaterMark(nullptr));

    int rc = to_proto<shrapnel_midi_mapping_MappingList>(message.mappings,
                                                         out.mappings);
    if(rc != 0)
    {
        return -1;
    }
    return 0;
}

template <>
std::optional<midi::GetResponse>
from_proto(const shrapnel_midi_mapping_GetResponse &message)
{
    midi::GetResponse out{};
    auto mappings = from_proto<etl::map<midi::Mapping::id_t, midi::Mapping, 10>>(
        message.mappings);
    if(!mappings.has_value())
    {
        return std::nullopt;
    }
    out.mappings = *mappings;
    return out;
}

template <>
int to_proto(const midi::CreateRequest &message,
             shrapnel_midi_mapping_CreateRequest &out)
{
    int rc = to_proto<shrapnel_midi_mapping_MappingRecord>(message.mapping,
                                                           out.mapping);
    if(rc != 0)
    {
        return -1;
    }
    return 0;
}

template <>
std::optional<midi::CreateRequest>
from_proto(const shrapnel_midi_mapping_CreateRequest &message)
{
    midi::CreateRequest out;
    auto mapping = from_proto<std::pair<midi::Mapping::id_t, midi::Mapping>>(
        message.mapping);
    if(!mapping.has_value())
    {
        return std::nullopt;
    }
    out.mapping = *mapping;
    return out;
}

template <>
int to_proto(const midi::CreateResponse &message,
             shrapnel_midi_mapping_CreateResponse &out)
{
    int rc = to_proto<shrapnel_midi_mapping_MappingRecord>(message.mapping,
                                                           out.mapping);
    if(rc != 0)
    {
        return -1;
    }
    return 0;
}

template <>
std::optional<midi::CreateResponse>
from_proto(const shrapnel_midi_mapping_CreateResponse &message)
{
    midi::CreateResponse out{};
    auto mapping = from_proto<std::pair<midi::Mapping::id_t, midi::Mapping>>(
        message.mapping);
    if(!mapping.has_value())
    {
        return std::nullopt;
    }
    out.mapping = *mapping;
    return out;
}

template <>
int to_proto(const midi::Update &message, shrapnel_midi_mapping_Update &out)
{
    int rc = to_proto<shrapnel_midi_mapping_MappingRecord>(message.mapping,
                                                           out.mapping);
    if(rc != 0)
    {
        return -1;
    }
    return 0;
}

template <>
std::optional<midi::Update>
from_proto(const shrapnel_midi_mapping_Update &message)
{
    midi::Update out{};
    auto mapping = from_proto<std::pair<midi::Mapping::id_t, midi::Mapping>>(
        message.mapping);
    if(!mapping.has_value())
    {
        return std::nullopt;
    }
    out.mapping = *mapping;
    return out;
}

template <>
int to_proto(const midi::Remove &message, shrapnel_midi_mapping_Remove &out)
{
    int rc = to_proto<shrapnel_uuid_Uuid>(message.id, out.id);
    if(rc != 0)
    {
        return -1;
    }
    return 0;
}

template <>
std::optional<midi::Remove>
from_proto(const shrapnel_midi_mapping_Remove &message)
{
    midi::Remove out{};
    auto id = from_proto<midi::Mapping::id_t>(message.id);
    if(!id.has_value())
    {
        return std::nullopt;
    }
    out.id = *id;
    return out;
}

template <>
int to_proto(const midi::Message::NoteOn &message,
             shrapnel_midi_mapping_MidiMessage_NoteOn &out)
{
    out.note = message.note;
    out.velocity = message.velocity;
    return 0;
}

template <>
std::optional<midi::Message::NoteOn>
from_proto(const shrapnel_midi_mapping_MidiMessage_NoteOn &message)
{
    midi::Message::NoteOn out{};
    out.note = message.note;
    out.velocity = message.velocity;
    return out;
}

template <>
int to_proto(const midi::Message::NoteOff &message,
             shrapnel_midi_mapping_MidiMessage_NoteOff &out)
{
    out.note = message.note;
    out.velocity = message.velocity;
    return 0;
}

template <>
std::optional<midi::Message::NoteOff>
from_proto(const shrapnel_midi_mapping_MidiMessage_NoteOff &message)
{
    midi::Message::NoteOff out{};
    out.note = message.note;
    out.velocity = message.velocity;
    return out;
}

template <>
int to_proto(const midi::Message::ControlChange &message,
             shrapnel_midi_mapping_MidiMessage_ControlChange &out)
{
    out.control = message.control;
    out.value = message.value;
    return 0;
}

template <>
std::optional<midi::Message::ControlChange>
from_proto(const shrapnel_midi_mapping_MidiMessage_ControlChange &message)
{
    midi::Message::ControlChange out{};
    out.control = message.control;
    out.value = message.value;
    return out;
}

template <>
int to_proto(const midi::Message::ProgramChange &message,
             shrapnel_midi_mapping_MidiMessage_ProgramChange &out)
{
    out.number = message.number;
    return 0;
}

template <>
std::optional<midi::Message::ProgramChange>
from_proto(const shrapnel_midi_mapping_MidiMessage_ProgramChange &message)
{
    midi::Message::ProgramChange out{};
    out.number = message.number;
    return out;
}

template <>
int to_proto(const midi::Message &message,
             shrapnel_midi_mapping_MidiMessage &out)
{
    auto channel = message.channel;
    return std::visit(
        [channel, &out](const auto &message) -> int
        {
            using T = std::decay_t<decltype(message)>;

            out.channel = channel;

            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, midi::Message::NoteOn>)
            {
                out.which_parameters =
                    shrapnel_midi_mapping_MidiMessage_note_on_tag;
                int rc = to_proto<shrapnel_midi_mapping_MidiMessage_NoteOn>(
                    message, out.parameters.note_on);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, midi::Message::NoteOff>)
            {
                out.which_parameters =
                    shrapnel_midi_mapping_MidiMessage_note_off_tag;
                int rc = to_proto<shrapnel_midi_mapping_MidiMessage_NoteOff>(
                    message, out.parameters.note_off);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, midi::Message::ControlChange>)
            {
                out.which_parameters =
                    shrapnel_midi_mapping_MidiMessage_control_change_tag;
                int rc =
                    to_proto<shrapnel_midi_mapping_MidiMessage_ControlChange>(
                        message, out.parameters.control_change);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, midi::Message::ProgramChange>)
            {
                out.which_parameters =
                    shrapnel_midi_mapping_MidiMessage_program_change_tag;
                int rc =
                    to_proto<shrapnel_midi_mapping_MidiMessage_ProgramChange>(
                        message, out.parameters.program_change);
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
        message.parameters);
}

template <>
std::optional<midi::Message>
from_proto(const shrapnel_midi_mapping_MidiMessage &message)
{
    midi::Message out{
        .channel = static_cast<uint8_t>(message.channel),
    };

    std::optional<decltype(out.parameters)> parameters;

    switch(message.which_parameters)
    {
    case shrapnel_midi_mapping_MidiMessage_note_on_tag:
        parameters =
            from_proto<midi::Message::NoteOn>(message.parameters.note_on);
        break;
    case shrapnel_midi_mapping_MidiMessage_note_off_tag:
        parameters =
            from_proto<midi::Message::NoteOff>(message.parameters.note_off);
        break;
    case shrapnel_midi_mapping_MidiMessage_control_change_tag:
        parameters = from_proto<midi::Message::ControlChange>(
            message.parameters.control_change);
        break;
    case shrapnel_midi_mapping_MidiMessage_program_change_tag:
        parameters = from_proto<midi::Message::ProgramChange>(
            message.parameters.program_change);
        break;
    default:
        return std::nullopt;
    }

    if(!parameters.has_value())
    {
        return std::nullopt;
    }

    out.parameters = *parameters;
    return out;
}

template <>
int to_proto(const midi::MessageReceived &message,
             shrapnel_midi_mapping_MessageReceived &out)
{
    int rc = to_proto<shrapnel_midi_mapping_MidiMessage>(message.message,
                                                         out.received_message);
    if(rc != 0)
    {
        return -1;
    }
    return 0;
}

template <>
std::optional<midi::MessageReceived>
from_proto(const shrapnel_midi_mapping_MessageReceived &message)
{
    midi::MessageReceived out{};
    auto received_message = from_proto<midi::Message>(message.received_message);
    out.message = *received_message;
    return out;
}

template <>
int to_proto(const midi::MappingApiMessage &message,
             shrapnel_midi_mapping_Message &out)
{
    ESP_LOGE("DEBUG",
             "%s stack %d",
             __FUNCTION__,
             uxTaskGetStackHighWaterMark(nullptr));

    return std::visit(
        [&out](const auto &message) -> int
        {
            ESP_LOGE("DEBUG",
                     "%s stack %d",
                     __FUNCTION__,
                     uxTaskGetStackHighWaterMark(nullptr));

            using T = std::decay_t<decltype(message)>;
            if constexpr(std::is_same_v<T, midi::GetRequest>)
            {
                out.which_message =
                    shrapnel_midi_mapping_Message_get_request_tag;
                int rc = to_proto<shrapnel_midi_mapping_GetRequest>(
                    message, out.message.get_request);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, midi::GetResponse>)
            {
                out.which_message =
                    shrapnel_midi_mapping_Message_get_response_tag;
                int rc = to_proto<shrapnel_midi_mapping_GetResponse>(
                    message, out.message.get_response);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, midi::CreateRequest>)
            {
                out.which_message =
                    shrapnel_midi_mapping_Message_create_request_tag;
                int rc = to_proto<shrapnel_midi_mapping_CreateRequest>(
                    message, out.message.create_request);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, midi::CreateResponse>)
            {
                out.which_message =
                    shrapnel_midi_mapping_Message_create_response_tag;
                int rc = to_proto<shrapnel_midi_mapping_CreateResponse>(
                    message, out.message.create_response);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, midi::Update>)
            {
                out.which_message = shrapnel_midi_mapping_Message_update_tag;
                int rc = to_proto<shrapnel_midi_mapping_Update>(
                    message, out.message.update);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, midi::Remove>)
            {
                out.which_message = shrapnel_midi_mapping_Message_remove_tag;
                int rc = to_proto<shrapnel_midi_mapping_Remove>(
                    message, out.message.remove);
                if(rc != 0)
                {
                    return -1;
                }
            }
            else if constexpr(std::is_same_v<T, midi::MessageReceived>)
            {
                out.which_message =
                    shrapnel_midi_mapping_Message_message_received_tag;
                int rc = to_proto<shrapnel_midi_mapping_MessageReceived>(
                    message, out.message.message_received);
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
std::optional<midi::MappingApiMessage>
from_proto(const shrapnel_midi_mapping_Message &message)
{
    switch(message.which_message)
    {
    case shrapnel_midi_mapping_Message_get_request_tag:
        return from_proto<midi::GetRequest>(message.message.get_request);
    case shrapnel_midi_mapping_Message_get_response_tag:
        return from_proto<midi::GetResponse>(message.message.get_response);
    case shrapnel_midi_mapping_Message_create_request_tag:
        return from_proto<midi::CreateRequest>(message.message.create_request);
    case shrapnel_midi_mapping_Message_create_response_tag:
        return from_proto<midi::CreateResponse>(
            message.message.create_response);
    case shrapnel_midi_mapping_Message_update_tag:
        return from_proto<midi::Update>(message.message.update);
    case shrapnel_midi_mapping_Message_remove_tag:
        return from_proto<midi::Remove>(message.message.remove);
    case shrapnel_midi_mapping_Message_message_received_tag:
        return from_proto<midi::MessageReceived>(
            message.message.message_received);
    }

    return std::nullopt;
}

} // namespace api

} // namespace shrapnel
