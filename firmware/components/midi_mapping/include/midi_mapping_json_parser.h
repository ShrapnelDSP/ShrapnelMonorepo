#pragma once

// Disable warning inside rapidjson
// https://github.com/Tencent/rapidjson/issues/1700
#include "audio_param.h"
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

namespace shrapnel {
namespace midi {

struct GetRequest {};

struct GetResponse {
    //etl::imap mappings,

    std::strong_ordering operator<=>(const GetResponse &other) const = default;
};

struct CreateRequest {
    std::pair<const Mapping::id_t, Mapping> mapping;

    std::strong_ordering operator<=>(const CreateRequest &other) const = default;
};

struct CreateResponse {
    std::pair<const Mapping::id_t, Mapping> mapping;

    std::strong_ordering operator<=>(const CreateResponse &other) const = default;
};

struct Update {
    std::pair<const Mapping::id_t, Mapping> mapping;

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
                // TODO move this parser to a function
                auto mapping_member = document.FindMember("mapping");
                if(mapping_member == document.MemberEnd()) {
                    ESP_LOGE(TAG, "mapping is missing");
                    goto error;
                }

                auto &mapping = mapping_member->value;
                if(!mapping.IsObject())
                {
                    ESP_LOGE(TAG, "mapping is not object");
                    goto error;
                }

                // XXX There should be only one key, so we take the first one
                //     and ignore the rest
                auto mapping_entry_member = mapping.GetObject().begin();

                if(mapping_entry_member == mapping.GetObject().end())
                {
                    ESP_LOGE(TAG, "mapping is empty");
                    goto error;
                }

                //auto &mapping_id = mapping_entry_member->name;
                auto &mapping_entry = mapping_entry_member->value;

                if(!mapping_entry.HasMember("midi_channel")) {
                    ESP_LOGE(TAG, "midi_channel is missing");
                    goto error;
                }

                if(!mapping_entry.HasMember("cc_number")) {
                    ESP_LOGE(TAG, "cc_number is missing");
                    goto error;
                }

                if(!mapping_entry.HasMember("parameter_id")) {
                    ESP_LOGE(TAG, "parameter_id is missing");
                    goto error;
                }


                //return CreateRequest{{{0}, {0, 0, Mapping::id_t("")}}};
                return CreateRequest({
                        Mapping::id_t{
                            0, 1,  2,  3,  4,  5,  6,  7,
                        },
                        Mapping{1, 2, parameters::id_t("gain")}}
                    );
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
