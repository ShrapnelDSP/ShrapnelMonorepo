#pragma once

#include "cmd_handling_api.h"
#include "rapidjson/document.h"
#include <optional>

// TODO move the template JSON function declarations to a common JSON header

namespace shrapnel::parameters {

template<typename T>
std::optional<T> from_json(const rapidjson::Value &json);

template<>
std::optional<Update> from_json(const rapidjson::Value &json);

template<>
std::optional<Initialise> from_json(const rapidjson::Value &json);

template<>
std::optional<ApiMessage> from_json(const rapidjson::Value &json);


template<typename T>
constexpr const char *get_message_type();

template<>
constexpr const char *get_message_type<Update>() {
    return "parameterUpdate";
}

template<>
constexpr const char *get_message_type<Initialise>() {
    return "initialiseParameters";
}

}
