#pragma once

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
    static MappingApiMessage from_json(const char *str) {
        rapidjson::Document document;
        document.Parse(str);

        if(document.HasParseError()) {
            ESP_LOGE(TAG, "failed to parse message");
            goto error;
        }

        if(!document.HasMember("messageType")) {
            ESP_LOGE(TAG, "messageType is missing");
            goto error;
        }

        {
            const char *message_type = document["messageType"].GetString();

            if(0 == strcmp(message_type, "MidiMap::get::request")) {
                return GetRequest();
            }
#if 0
            else if(0 == strcmp(message_type, "MidiMap::create::request") {
#if 0
                if(!document.hasMember("mapping")) {
                    ESP_LOGE(TAG, "midi_channel is missing");
                    goto error;
                }

                auto &mapping = document.getObject();

                if(!document.hasMember("midi_channel")) {
                    ESP_LOGE(TAG, "midi_channel is missing");
                    goto error;
                }

                if(!document.hasMember("cc_number")) {
                    ESP_LOGE(TAG, "cc_number is missing");
                    goto error;
                }

                if(!document.hasMember("parameter_id")) {
                    ESP_LOGE(TAG, "parameter_id is missing");
                    goto error;
                }


                return CreateRequest{{0},};
#endif
            }

#if 0
            if(0 == strcmp("MidiMap::update")
            if(0 == strcmp("MidiMap::remove")
#endif
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
