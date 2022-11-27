#include "cmd_handling_json_builder.h"

#include "cmd_handling_json.h"
#include "cmd_handling_api.h"
#include "esp_log.h"

namespace shrapnel::parameters {

template<>
rapidjson::Value to_json(rapidjson::Document &document, const Update &object)
{
    rapidjson::Value json;
    json.SetObject();

    rapidjson::Value id;
    id.SetString(object.id.data(), object.id.size(), document.GetAllocator());
    json.AddMember("id", id, document.GetAllocator());

    rapidjson::Value value;
    value = object.value;
    json.AddMember("value", value, document.GetAllocator());

    return json;
}

template<>
rapidjson::Value to_json(rapidjson::Document &document, const ApiMessage &object)
{
    static constexpr char TAG[] = "parameters::ApiMessage to json";
    rapidjson::Value json;

    auto visitor = [&](const auto &message) {
        using T = std::decay_t<decltype(message)>;

        if constexpr (std::is_same_v<T, Update>) {
            json = to_json(document, message);
        } else {
            ESP_LOGE(TAG, "No handler registered for message");
        }

        json.AddMember("messageType", rapidjson::StringRef(get_message_type<T>()), document.GetAllocator());
    };

    std::visit(visitor, object);

    return json;
}

}
