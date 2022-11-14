#pragma once

#include "midi_mapping_json_builder.h"

namespace shrapnel {
namespace midi {

// TODO put full specialisations in a cpp file to avoid ODR violation
template<>
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

    rapidjson::Value parameter_id;
    parameter_id.SetString(object.parameter_name.data(), object.parameter_name.size(), document.GetAllocator());
    json.AddMember("parameter_id", parameter_id, document.GetAllocator());

    return json;
}

#if 0
template<>
rapidjson::Value to_json(rapidjson::Document &document, const GetResponse &object)
#endif

}
}
