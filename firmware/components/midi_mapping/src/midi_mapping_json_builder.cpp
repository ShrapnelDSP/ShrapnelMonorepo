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

#include "midi_mapping_json_builder.h"
#include "midi_mapping_api.h"
#include "midi_message_type.h"
#include "rapidjson/document.h"
#include <variant>

namespace {
constexpr char TAG[] = "midi_mapping_json_builder";
};

namespace shrapnel {
namespace midi {

template <>
rapidjson::Value to_json(rapidjson::Document &document, const Message &object)
{
    rapidjson::Value json;
    json.SetObject();

    json.AddMember("channel", object.channel, document.GetAllocator());

    std::visit(
        [&](auto &parameters)
        {
            using T = std::decay_t<decltype(parameters)>;

            if constexpr(std::is_same_v<T, Message::NoteOn>)
            {
                json.AddMember("runtimeType",
                               rapidjson::StringRef("noteOn"),
                               document.GetAllocator());
                json.AddMember(
                    "note", parameters.note, document.GetAllocator());
                json.AddMember(
                    "velocity", parameters.velocity, document.GetAllocator());
            }
            else if constexpr(std::is_same_v<T, Message::NoteOff>)
            {
                json.AddMember("runtimeType",
                               rapidjson::StringRef("noteOff"),
                               document.GetAllocator());
                json.AddMember(
                    "note", parameters.note, document.GetAllocator());
                json.AddMember(
                    "velocity", parameters.velocity, document.GetAllocator());
            }
            else if constexpr(std::is_same_v<T, Message::ControlChange>)
            {
                json.AddMember("runtimeType",
                               rapidjson::StringRef("controlChange"),
                               document.GetAllocator());
                json.AddMember(
                    "control", parameters.control, document.GetAllocator());
                json.AddMember(
                    "value", parameters.value, document.GetAllocator());
            }
            else if constexpr(std::is_same_v<T, Message::ProgramChange>)
            {
                json.AddMember("runtimeType",
                               rapidjson::StringRef("programChange"),
                               document.GetAllocator());
                json.AddMember(
                    "number", parameters.number, document.GetAllocator());
            }
        },
        object.parameters);

    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document, const Mapping &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value midi_channel;
    midi_channel = object.midi_channel;
    json.AddMember("midi_channel", midi_channel, document.GetAllocator());

    rapidjson::Value cc_number;
    cc_number = object.cc_number;
    json.AddMember("cc_number", cc_number, document.GetAllocator());

    rapidjson::Value mode;
    switch(object.mode)
    {
    case Mapping::Mode::PARAMETER:
        mode.SetString(rapidjson::StringRef("parameter"));
        break;
    case Mapping::Mode::TOGGLE:
        mode.SetString(rapidjson::StringRef("toggle"));
        break;
    case Mapping::Mode::BUTTON:
        mode.SetString(rapidjson::StringRef("button"));
        break;
    }
    json.AddMember("mode", mode, document.GetAllocator());

    if(object.parameter_name.has_value())
    {
        rapidjson::Value parameter_id;
        parameter_id.SetString(object.parameter_name->data(),
                               object.parameter_name->size(),
                               document.GetAllocator());
        json.AddMember("parameter_id", parameter_id, document.GetAllocator());
    }

    if(object.preset_id.has_value())
    {
        json.AddMember<unsigned int>(
            "preset_id", *object.preset_id, document.GetAllocator());
    }

    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const std::pair<Mapping::id_t, Mapping> &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value mapping = to_json(document, object.second);
    auto id = uuid::to_json(document, object.first);
    json.AddMember(id, mapping, document.GetAllocator());
    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const CreateResponse &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value mapping = to_json(document, object.mapping);
    json.AddMember("mapping", mapping, document.GetAllocator());
    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const GetResponse &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value mappings = to_json(document, *object.mappings);
    json.AddMember("mappings", mappings, document.GetAllocator());
    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const MessageReceived &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value message = to_json(document, object.message);
    json.AddMember("message", message, document.GetAllocator());
    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const MappingApiMessage &object)
{
    rapidjson::Value json;

    auto visitor = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, CreateResponse> ||
                     std::is_same_v<T, GetResponse> ||
                     std::is_same_v<T, MessageReceived>)
        {
            json = to_json(document, message);
        }
        else
        {
            ESP_LOGE(TAG, "No handler registered for message");
        }

        json.AddMember("messageType",
                       rapidjson::StringRef(get_message_type<T>()),
                       document.GetAllocator());
    };

    std::visit(visitor, object);

    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const etl::imap<Mapping::id_t, Mapping> &object)
{
    rapidjson::Value json;
    json.SetObject();

    for(const auto &entry : object)
    {
        rapidjson::Value mapping = to_json(document, entry.second);
        auto id = uuid::to_json(document, entry.first);
        json.AddMember(id, mapping, document.GetAllocator());
    }

    return json;
}

} // namespace midi
} // namespace shrapnel
