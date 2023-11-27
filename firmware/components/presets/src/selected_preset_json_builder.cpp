#include "selected_preset_json_builder.h"

namespace shrapnel::selected_preset {

template <>
rapidjson::Value to_json(rapidjson::Document &document, const Notify &object)
{
    rapidjson::Value json;
    json.SetObject();

    json.AddMember("messageType",
                   rapidjson::StringRef("SelectedPreset::notify"),
                   document.GetAllocator());

    json.AddMember("selectedPreset",
                   uuid::to_json(document, object.selectedPresetId),
                   document.GetAllocator());
    
    return json;
}

} // namespace shrapnel::selected_preset