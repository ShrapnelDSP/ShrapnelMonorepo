#include "selected_preset_json_builder.h"
#include "selected_preset_message_type.h"

namespace shrapnel::selected_preset {

template <>
rapidjson::Value to_json(rapidjson::Document &document, const Notify &object)
{
    rapidjson::Value json;
    json.SetObject();

    json.AddMember<unsigned int>(
        "selectedPreset", object.selectedPresetId, document.GetAllocator());

    return json;
}

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const SelectedPresetApiMessage &object)
{
    rapidjson::Value json;

    auto visitor = [&](const auto &message)
    {
        using T = std::decay_t<decltype(message)>;

        if constexpr(std::is_same_v<T, Notify>)
        {
            json = to_json(document, message);
        }

        json.AddMember("messageType",
                       rapidjson::StringRef(get_message_type<T>()),
                       document.GetAllocator());
    };

    std::visit(visitor, object);

    return json;
}

} // namespace shrapnel::selected_preset