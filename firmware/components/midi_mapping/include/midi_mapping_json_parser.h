#pragma once

#include "audio_param.h"
#include <cstring>

// Disable warning inside rapidjson
// https://github.com/Tencent/rapidjson/issues/1700
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#pragma GCC diagnostic pop

#include "midi_mapping.h"
#include <variant>
#include <optional>

namespace shrapnel {
namespace midi {


template<typename T1, typename T2>
etl::string_stream& operator<<(etl::string_stream& out, const std::pair<T1, T2>& pair);

template<typename T, std::size_t MAX_SIZE>
etl::string_stream& operator<<(etl::string_stream& out, const std::array<T, MAX_SIZE>& array);

/// return non-zero on error
int parse_uuid(Mapping::id_t &uuid, const char *string)
{
    constexpr std::size_t UUID_LENGTH = 36;
    constexpr char TAG[] = "parse_uuid";

    if(UUID_LENGTH != std::strlen(string))
    {
        ESP_LOGE(TAG, "Incorrect UUID string length");
        return -1;
    }

    size_t i = 0;
    size_t j = 0;
    ESP_LOGD(TAG, "i = %zu, j = %zu", i , j);
    while(i < UUID_LENGTH)
    {
        char digit[2];
        std::memcpy(digit, &string[i], 2);

        ESP_LOGD(TAG, "digit = %c%c", digit[0] , digit[1]);

        // TODO error on invalid characters: z, symbols etc
        //      The only valid characters are 0 to 9 and a to f.
        //
        //      Handle uppercase as well. It is required when a string UUID
        //      is an input as per RFC 4122 Section 3
        //      https://tools.ietf.org/html/rfc4122#section-3
        auto get_value = [&] (char hex) -> uint8_t {
            if(hex >= 'a')
            {
                return hex - 'a' + 10;
            }
            else
            {
                return hex - '0';
            }
        };

        uuid[j] = get_value(digit[0]) << 4 | get_value(digit[1]);

        j++;

        i += 2;
        bool is_separator = (i == 8) || (i == 13) || (i == 18) || (i == 23);
        if(is_separator) i++;
        ESP_LOGD(TAG, "i = %zu, j = %zu", i , j);
    }

    return 0;
}

struct GetRequest {};

struct GetResponse {
    //etl::imap mappings,

    std::strong_ordering operator<=>(const GetResponse &other) const = default;
};

struct CreateRequest {
    std::pair<Mapping::id_t, Mapping> mapping;

    std::strong_ordering operator<=>(const CreateRequest &other) const = default;

    friend etl::string_stream& operator<<(etl::string_stream&  out, const CreateRequest& self) {
        out << "{ " << self.mapping << " }";
        return out;
    }

    static std::optional<CreateRequest> from_json(const rapidjson::Value &json)
    {
        if(!json.IsObject())
        {
            ESP_LOGE(TAG, "mapping is not object");
            return std::nullopt;
        }

        // XXX There should be only one key, so we take the first one
        //     and ignore the rest
        auto mapping_entry_member = json.GetObject().begin();

        if(mapping_entry_member == json.GetObject().end())
        {
            ESP_LOGE(TAG, "mapping is empty");
            return std::nullopt;
        }

        auto &mapping_id = mapping_entry_member->name;
        auto &mapping_entry = mapping_entry_member->value;

        auto midi_channel = mapping_entry.FindMember("midi_channel");
        if(midi_channel == mapping_entry.MemberEnd()) {
            ESP_LOGE(TAG, "midi_channel is missing");
            return std::nullopt;
        }

        auto cc_number = mapping_entry.FindMember("cc_number");
        if(cc_number == mapping_entry.MemberEnd()) {
            ESP_LOGE(TAG, "cc_number is missing");
            return std::nullopt;
        }

        auto parameter_id = mapping_entry.FindMember("parameter_id");
        if(parameter_id == mapping_entry.MemberEnd()) {
            ESP_LOGE(TAG, "parameter_id is missing");
            return std::nullopt;
        }

        // TODO range check before narrowing conversion to uint8_t
        auto out = CreateRequest({
                Mapping::id_t{0},
                Mapping{
                static_cast<uint8_t>(midi_channel->value.GetInt()),
                static_cast<uint8_t>(cc_number->value.GetInt()),
                parameters::id_t(parameter_id->value.GetString())}}
            );

        parse_uuid(out.mapping.first, mapping_id.GetString());

        return out;
    }

    static constexpr char TAG[] = "CreateRequest";
};

struct CreateResponse {
    std::pair<Mapping::id_t, Mapping> mapping;

    std::strong_ordering operator<=>(const CreateResponse &other) const = default;
};

struct Update {
    std::pair<Mapping::id_t, Mapping> mapping;

    std::strong_ordering operator<=>(const Update &other) const = default;
};

struct Remove {
    Mapping::id_t id;

    std::strong_ordering operator<=>(const Remove &other) const = default;
};

using MappingApiMessage = std::variant<std::monostate, GetRequest, GetResponse, CreateRequest, CreateResponse, Update, Remove>;

class MappingApiMessageBuilder final {
    public:

    /** Convert the message into a object represting it.
     *
     * \return The output variant will contain \p std::monostate on an error.
     */
    static MappingApiMessage from_json(const char *str) {
        rapidjson::Document document;
        document.Parse(str);

        if(document.HasParseError()) {
            ESP_LOGE(TAG, "failed to parse message");
            return std::monostate();
        }

        auto message_type_member = document.FindMember("messageType");
        if(message_type_member == document.MemberEnd()) {
            ESP_LOGE(TAG, "messageType is missing");
            goto error;
        }

        if(!message_type_member->value.IsString()) {
            ESP_LOGE(TAG, "messageType is not string");
            goto error;
        }

        {
            const char *message_type = message_type_member->value.GetString();

            if(0 == strcmp(message_type, "MidiMap::get::request")) {
                return GetRequest();
            }
            else if(0 == strcmp(message_type, "MidiMap::create::request")) {
                auto mapping_member = document.FindMember("mapping");
                if(mapping_member == document.MemberEnd()) {
                    ESP_LOGE(TAG, "mapping is missing");
                    goto error;
                }

                auto &mapping = mapping_member->value;
                auto out = CreateRequest::from_json(mapping);
                if(out.has_value())
                {
                    return *out;
                }

                return std::monostate();
            }

#if 0
            if(0 == strcmp("MidiMap::update")
            if(0 == strcmp("MidiMap::remove")
#endif
        }

error:
        return std::monostate();
    }

    private:
    static constexpr char TAG[] = "midi_mapping_api_message_builder";
};

}
}
