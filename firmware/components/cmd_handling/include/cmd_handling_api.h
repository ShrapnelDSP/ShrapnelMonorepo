#pragma once

#include "audio_param.h"
#include <variant>

namespace shrapnel::parameters {

struct Update final {
    id_t id;
    float value;

    std::strong_ordering operator<=>(const Update &other) const = default;
};

struct Initialise final {};

using ApiMessage = std::variant<Update, Initialise>;

}
