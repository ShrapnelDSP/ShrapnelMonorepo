#pragma once

#include "cmd_handling_api.h"
#include "rapidjson/document.h"

namespace shrapnel::parameters {

template<typename T>
rapidjson::Value to_json(rapidjson::Document &document, const T &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const Update &object);

}
