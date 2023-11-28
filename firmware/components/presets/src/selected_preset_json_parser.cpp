#include "selected_preset_json_parser.h"
#include "esp_log.h"
#include "selected_preset_message_type.h"
#include <etl/map.h>

namespace shrapnel::selected_preset {

template <>
std::optional<Read> from_json(const rapidjson::Value &json)
{
    return Read{};
}

template <>
std::optional<Write> from_json(const rapidjson::Value &json)
{
    auto id_json = json.FindMember("selectedPreset");
    if(id_json == json.MemberEnd())
    {
        return std::nullopt;
    }

    auto id = uuid::from_json<uuid::uuid_t>(id_json->value);
    if(!id.has_value())
    {
        return std::nullopt;
    }

    return Write{.selectedPresetId{*id}};
}

template <>
std::optional<SelectedPresetApiMessage> from_json(const rapidjson::Value &json)
{
    constexpr char TAG[] = "SelectedPresetApiMessage from_json";

    auto message_type_member = json.FindMember("messageType");
    if(message_type_member == json.MemberEnd())
    {
        ESP_LOGE(TAG, "messageType is missing");
        goto error;
    }

    if(!message_type_member->value.IsString())
    {
        ESP_LOGE(TAG, "messageType is not string");
        goto error;
    }

    {
        const etl::map<std::string,
                       std::function<std::optional<SelectedPresetApiMessage>()>,
                       2>
            lut{
                {get_message_type<Read>(),
                 [&] { return from_json<Read>(json); }},
                {get_message_type<Write>(),
                 [&] { return from_json<Write>(json); }},
            };

        const char *message_type = message_type_member->value.GetString();
        if(auto f = lut.find(message_type); f != lut.end())
        {
            return f->second();
        }
    }

error:
    return std::nullopt;
}

} // namespace shrapnel::selected_preset
