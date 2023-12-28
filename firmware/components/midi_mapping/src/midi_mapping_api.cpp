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

#if 0
etl::string_stream &operator<<(etl::string_stream &out,
                               const MappingApiMessage &self)
{
    std::visit(
        [&](const auto &message)
        {
            using T = std::decay_t<decltype(message)>;

            if constexpr(std::is_same_v<T, GetRequest>)
            {
                out << "<GetRequest>" << message;
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
                ESP_LOGE("DEBUG", "unknown ?!");
                out << "Unknown";
            }
        },
        self);

    return out;
}
#endif

etl::string_stream &operator<<(etl::string_stream &out,
                               const MappingApiMessage &self)
{

// FIXME: neither version is working in the firmware build
#if 0
    auto print = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, GetRequest>)
        {
            out << "<GetRequest>" << message;
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
    else if(auto message = std::get_if<MessageReceived>(&self))
    {
        out << "<MessageReceived>" << *message;
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
to_bytes(const shrapnel_midi_mapping_Mapping &message,
         std::span<uint8_t> buffer)
{
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.data(), buffer.size());

    bool success =
        pb_encode(&stream, &shrapnel_midi_mapping_Mapping_msg, &message);
    if(!success)
    {
        return std::nullopt;
    }

    return buffer.subspan(0, stream.bytes_written);
}

template <>
std::optional<shrapnel_midi_mapping_Mapping>
from_bytes(std::span<const uint8_t> buffer)
{
    auto stream = pb_istream_from_buffer(buffer.data(), buffer.size());
    shrapnel_midi_mapping_Mapping message =
        shrapnel_midi_mapping_Mapping_init_zero;
    bool success =
        pb_decode(&stream, &shrapnel_midi_mapping_Mapping_msg, &message);
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
    default:
        return -1;
    }
    strncpy(out.parameterName,
            message.parameter_name.data(),
            sizeof out.parameterName);
    return 0;
}

template <>
int from_proto(const shrapnel_midi_mapping_Mapping &message, midi::Mapping &out)
{
    out.midi_channel = static_cast<uint8_t>(message.midi_channel);
    out.cc_number = static_cast<uint8_t>(message.cc_number);
    out.parameter_name = message.parameterName;
    switch(message.mode)
    {
    case shrapnel_midi_mapping_Mapping_Mode_parameter:
        out.mode = midi::Mapping::Mode::PARAMETER;
        break;
    case shrapnel_midi_mapping_Mapping_Mode_toggle:
        out.mode = midi::Mapping::Mode::TOGGLE;
        break;
    default:
        return -1;
    }
    return 0;
}

template <>
int to_proto(const std::pair<midi::Mapping::id_t, midi::Mapping> &message,
             shrapnel_midi_mapping_MappingRecord &out)
{
    int rc =
        to_proto<shrapnel_midi_mapping_Mapping>(message.second, out.mapping);
    if(rc != 0)
    {
        return -1;
    }
    out.has_mapping = true;
    out.id = message.first;
    return 0;
}

template <>
int from_proto(const shrapnel_midi_mapping_MappingRecord &message,
               std::pair<midi::Mapping::id_t, midi::Mapping> &out)
{
    out.first = message.id;
    return from_proto<midi::Mapping>(message.mapping, out.second);
}

template <>
int to_proto(const midi::GetRequest &, shrapnel_midi_mapping_GetRequest &)
{
    return 0;
}

template <>
int from_proto(const shrapnel_midi_mapping_GetRequest &, midi::GetRequest &)
{
    return 0;
}

template <>
int to_proto(const midi::CreateRequest &message,
             shrapnel_midi_mapping_CreateRequest &out)
{
    out.has_mapping = true;
    return to_proto<shrapnel_midi_mapping_Mapping>(message.mapping,
                                                   out.mapping);
}

template <>
int from_proto(const shrapnel_midi_mapping_CreateRequest &message,
               midi::CreateRequest &out)
{
    return from_proto<midi::Mapping>(message.mapping, out.mapping);
}

template <>
int to_proto(const midi::CreateResponse &message,
             shrapnel_midi_mapping_CreateResponse &out)
{
    out.has_mapping = true;
    return to_proto<shrapnel_midi_mapping_MappingRecord>(message.mapping,
                                                         out.mapping);
}

template <>
int from_proto(const shrapnel_midi_mapping_CreateResponse &message,
               midi::CreateResponse &out)
{
    return from_proto<std::pair<midi::Mapping::id_t, midi::Mapping>>(
        message.mapping, out.mapping);
}

template <>
int to_proto(const midi::Update &message, shrapnel_midi_mapping_Update &out)
{
    out.has_mapping = true;
    int rc = to_proto<shrapnel_midi_mapping_MappingRecord>(message.mapping,
                                                           out.mapping);
    if(rc != 0)
    {
        return -1;
    }
    return 0;
}

template <>
int from_proto(const shrapnel_midi_mapping_Update &message, midi::Update &out)
{
    return from_proto<std::pair<midi::Mapping::id_t, midi::Mapping>>(
        message.mapping, out.mapping);
}

template <>
int to_proto(const midi::Remove &message, shrapnel_midi_mapping_Remove &out)
{
    out.id = message.id;
    return 0;
}

template <>
int from_proto(const shrapnel_midi_mapping_Remove &message, midi::Remove &out)
{
    out.id = message.id;
    return 0;
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
int from_proto(const shrapnel_midi_mapping_MidiMessage_NoteOn &message,
               midi::Message::NoteOn &out)
{
    out.note = message.note;
    out.velocity = message.velocity;
    return 0;
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
int from_proto(const shrapnel_midi_mapping_MidiMessage_NoteOff &message,
               midi::Message::NoteOff &out)
{
    out.note = message.note;
    out.velocity = message.velocity;
    return 0;
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
int from_proto(const shrapnel_midi_mapping_MidiMessage_ControlChange &message,
               midi::Message::ControlChange &out)
{
    out.control = message.control;
    out.value = message.value;
    return 0;
}

template <>
int to_proto(const midi::Message::ProgramChange &message,
             shrapnel_midi_mapping_MidiMessage_ProgramChange &out)
{
    out.number = message.number;
    return 0;
}

template <>
int from_proto(const shrapnel_midi_mapping_MidiMessage_ProgramChange &message,
               midi::Message::ProgramChange &out)
{
    out.number = message.number;
    return 0;
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
                return to_proto<shrapnel_midi_mapping_MidiMessage_NoteOn>(
                    message, out.parameters.note_on);
            }
            else if constexpr(std::is_same_v<T, midi::Message::NoteOff>)
            {
                out.which_parameters =
                    shrapnel_midi_mapping_MidiMessage_note_off_tag;
                return to_proto<shrapnel_midi_mapping_MidiMessage_NoteOff>(
                    message, out.parameters.note_off);
            }
            else if constexpr(std::is_same_v<T, midi::Message::ControlChange>)
            {
                out.which_parameters =
                    shrapnel_midi_mapping_MidiMessage_control_change_tag;
                return to_proto<
                    shrapnel_midi_mapping_MidiMessage_ControlChange>(
                    message, out.parameters.control_change);
            }
            else if constexpr(std::is_same_v<T, midi::Message::ProgramChange>)
            {
                out.which_parameters =
                    shrapnel_midi_mapping_MidiMessage_program_change_tag;
                return to_proto<
                    shrapnel_midi_mapping_MidiMessage_ProgramChange>(
                    message, out.parameters.program_change);
            }

            return -1;
        },
        message.parameters);
}

template <>
int from_proto(const shrapnel_midi_mapping_MidiMessage &message,
               midi::Message &out)
{
    out.channel = message.channel;

    std::optional<decltype(out.parameters)> parameters;

    switch(message.which_parameters)
    {
    case shrapnel_midi_mapping_MidiMessage_note_on_tag:
    {
        midi::Message::NoteOn tmp{};
        int rc =
            from_proto<midi::Message::NoteOn>(message.parameters.note_on, tmp);
        out.parameters = tmp;
        return rc;
    }
    case shrapnel_midi_mapping_MidiMessage_note_off_tag:
    {
        midi::Message::NoteOff tmp{};
        int rc = from_proto<midi::Message::NoteOff>(message.parameters.note_off,
                                                    tmp);
        out.parameters = tmp;
        return rc;
    }
    case shrapnel_midi_mapping_MidiMessage_control_change_tag:
    {
        midi::Message::ControlChange tmp{};
        int rc = from_proto<midi::Message::ControlChange>(
            message.parameters.control_change, tmp);
        out.parameters = tmp;
        return rc;
    }
    case shrapnel_midi_mapping_MidiMessage_program_change_tag:
    {
        midi::Message::ProgramChange tmp{};
        int rc = from_proto<midi::Message::ProgramChange>(
            message.parameters.program_change, tmp);
        out.parameters = tmp;
        return rc;
    }
    default:
        return -1;
    }
}

template <>
int to_proto(const midi::MessageReceived &message,
             shrapnel_midi_mapping_MessageReceived &out)
{
    out.has_received_message = true;
    int rc = to_proto<shrapnel_midi_mapping_MidiMessage>(message.message,
                                                         out.received_message);
    if(rc != 0)
    {
        return -1;
    }
    out.has_received_message = true;
    return 0;
}

template <>
int from_proto(const shrapnel_midi_mapping_MessageReceived &message,
               midi::MessageReceived &out)
{
    return from_proto<midi::Message>(message.received_message, out.message);
}

template <>
int to_proto(const midi::MappingApiMessage &message,
             shrapnel_midi_mapping_Message &out)
{
    return std::visit(
        [&out](const auto &message) -> int
        {
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
int from_proto(const shrapnel_midi_mapping_Message &message,
               midi::MappingApiMessage &out)
{
    switch(message.which_message)
    {
    case shrapnel_midi_mapping_Message_get_request_tag:
    {
        midi::GetRequest tmp{};
        int rc = from_proto<midi::GetRequest>(message.message.get_request, tmp);
        out = tmp;
        return rc;
    }
    case shrapnel_midi_mapping_Message_create_request_tag:
    {
        midi::CreateRequest tmp{};
        int rc = from_proto<midi::CreateRequest>(message.message.create_request,
                                                 tmp);
        out = tmp;
        return rc;
    }
    case shrapnel_midi_mapping_Message_create_response_tag:
    {
        midi::CreateResponse tmp{};
        int rc = from_proto<midi::CreateResponse>(
            message.message.create_response, tmp);
        out = tmp;
        return rc;
    }
    case shrapnel_midi_mapping_Message_update_tag:
    {
        midi::Update tmp{};
        int rc = from_proto<midi::Update>(message.message.update, tmp);
        out = tmp;
        return rc;
    }
    case shrapnel_midi_mapping_Message_remove_tag:
    {
        midi::Remove tmp{};
        int rc = from_proto<midi::Remove>(message.message.remove, tmp);
        out = tmp;
        return rc;
    }
    case shrapnel_midi_mapping_Message_message_received_tag:
    {
        midi::MessageReceived tmp{};
        int rc = from_proto<midi::MessageReceived>(
            message.message.message_received, tmp);
        out = tmp;
        return rc;
    }
    }

    return -1;
}

template <>
std::optional<std::span<uint8_t>> to_bytes(const midi::Mapping &message,
                                           std::span<uint8_t> buffer)
{
    shrapnel_midi_mapping_Mapping proto;
    int rc = to_proto(message, proto);
    if(rc != 0)
    {
        return std::nullopt;
    }

    return to_bytes(proto, buffer);
}

template <>
std::optional<midi::Mapping> from_bytes(std::span<const uint8_t> buffer)
{
    auto proto = from_bytes<shrapnel_midi_mapping_Mapping>(buffer);
    if(!proto.has_value())
    {
        return std::nullopt;
    }

    midi::Mapping out{};
    int rc = from_proto(*proto, out);
    if(rc != 0)
    {
        return std::nullopt;
    }
    return out;
}

} // namespace api

} // namespace shrapnel
