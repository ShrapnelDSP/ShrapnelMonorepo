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

#include "midi_mapping_api.h"
#include "etl_utility.h"
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
std::optional<shrapnel_midi_mapping_Mapping>
to_proto(const midi::Mapping &message)
{
    shrapnel_midi_mapping_Mapping out = shrapnel_midi_mapping_Mapping_init_zero;
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
    return out;
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
std::optional<shrapnel_midi_mapping_MappingRecord>
to_proto(const std::pair<midi::Mapping::id_t, midi::Mapping> &message)
{
    shrapnel_midi_mapping_MappingRecord out =
        shrapnel_midi_mapping_MappingRecord_init_zero;
    auto mapping = from_proto<shrapnel_midi_mapping_Mapping>(message.second);
    if(!mapping.has_value())
    {
        return std::nullopt;
    }
    // FIXME: do we have to set the has_mapping etc? In proto3, there is no
    // presence, so it should not even exist?
    out.mapping = *mapping;
    auto id = to_proto<shrapnel_uuid_Uuid>(message.first);
    if(!id.has_value())
    {
        return std::nullopt;
    }
    out.id = *id;
    return out;
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
std::optional<shrapnel_midi_mapping_MappingList>
to_proto(const etl::map<midi::Mapping::id_t, midi::Mapping, 10> &message)
{
    shrapnel_midi_mapping_MappingList out =
        shrapnel_midi_mapping_MappingList_init_zero;

    int i = 0;
    for(const auto &[key, value] : message)
    {
        if(i >= (sizeof(out.mappings) / sizeof(out.mappings[0])))
        {
            break;
        }

        auto mapping = to_proto<shrapnel_midi_mapping_MappingRecord>(
            std::pair{key, value});
        if(!mapping.has_value())
        {
            return std::nullopt;
        }
        out.mappings[i] = *mapping;
        i++;
    }
    out.mappings_count = i;

    return out;
}

template <>
std::optional<etl::map<midi::Mapping::id_t, midi::Mapping, 10>>
from_proto(const shrapnel_midi_mapping_MappingList &message)
{
    etl::map<midi::Mapping::id_t, midi::Mapping, 10> out;

    for(size_t i = 0; i < message.mappings_count; i++)
    {
        auto &proto_mapping = message.mappings[i];
        // FIXME: move to uuid implementation: midi::Mapping::id_t id{std::to_array(proto_mapping.id.value)};
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
std::optional<shrapnel_midi_mapping_GetRequest>
to_proto(const midi::GetRequest &)
{
    shrapnel_midi_mapping_GetRequest out =
        shrapnel_midi_mapping_GetRequest_init_zero;
    return out;
}

template <>
std::optional<midi::GetRequest>
from_proto(const shrapnel_midi_mapping_GetRequest &)
{
    midi::GetRequest out{};
    return out;
}

template <>
std::optional<shrapnel_midi_mapping_GetResponse>
to_proto(const midi::GetResponse &message)
{
    shrapnel_midi_mapping_GetResponse out =
        shrapnel_midi_mapping_GetResponse_init_zero;
    auto mappings =
        to_proto<shrapnel_midi_mapping_MappingList>(message.mappings);
    if(!mappings.has_value())
    {
        return std::nullopt;
    }
    out.mappings = *mappings;
    return out;
}

template <>
std::optional<midi::GetResponse>
from_proto(const shrapnel_midi_mapping_GetResponse &message)
{
    midi::GetResponse out{};
    auto mappings =
        from_proto<etl::map<midi::Mapping::id_t, midi::Mapping, 10>>(
            message.mappings);
    if(!mappings.has_value())
    {
        return std::nullopt;
    }
    out.mappings = *mappings;
    return out;
}

template <>
std::optional<shrapnel_midi_mapping_CreateRequest>
to_proto(const midi::CreateRequest &message)
{
    shrapnel_midi_mapping_CreateRequest out =
        shrapnel_midi_mapping_CreateRequest_init_zero;
    auto mapping =
        to_proto<shrapnel_midi_mapping_MappingRecord>(message.mapping);
    if(!mapping.has_value())
    {
        return std::nullopt;
    }
    out.mapping = *mapping;
    return out;
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
std::optional<shrapnel_midi_mapping_CreateResponse>
to_proto(const midi::CreateResponse &message)
{
    shrapnel_midi_mapping_CreateResponse out =
        shrapnel_midi_mapping_CreateResponse_init_zero;

    auto mapping =
        to_proto<shrapnel_midi_mapping_MappingRecord>(message.mapping);
    if(!mapping.has_value())
    {
        return std::nullopt;
    }
    out.mapping = *mapping;
    return out;
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
std::optional<shrapnel_midi_mapping_Update>
to_proto(const midi::Update &message)
{
    shrapnel_midi_mapping_Update out = shrapnel_midi_mapping_Update_init_zero;

    auto mapping =
        to_proto<shrapnel_midi_mapping_MappingRecord>(message.mapping);
    if(!mapping.has_value())
    {
        return std::nullopt;
    }
    out.mapping = *mapping;
    return out;
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
std::optional<shrapnel_midi_mapping_Remove>
to_proto(const midi::Remove &message)
{
    shrapnel_midi_mapping_Remove out = shrapnel_midi_mapping_Remove_init_zero;

    auto id = to_proto<shrapnel_uuid_Uuid>(message.id);
    if(!id.has_value())
    {
        return std::nullopt;
    }
    out.id = *id;
    return out;
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
std::optional<shrapnel_midi_mapping_MidiMessage>
to_proto(const midi::Message &message)
{
#error
}

template <>
std::optional<midi::Message>
from_proto(const shrapnel_midi_mapping_MidiMessage &message)
{
#error
}

template <>
std::optional<shrapnel_midi_mapping_MessageReceived>
to_proto(const midi::MessageReceived &message)
{
    shrapnel_midi_mapping_MessageReceived out =
        shrapnel_midi_mapping_MessageReceived_init_zero;

    auto received_message =
        to_proto<shrapnel_midi_mapping_MidiMessage>(message.message);
    if(!received_message.has_value())
    {
        return std::nullopt;
    }
    out.received_message = *received_message;
    return out;
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
std::optional<shrapnel_midi_mapping_Message>
to_proto(const midi::MappingApiMessage &message);

template <>
std::optional<midi::MappingApiMessage>
from_proto(const shrapnel_midi_mapping_Message &message);

} // namespace api

} // namespace shrapnel
