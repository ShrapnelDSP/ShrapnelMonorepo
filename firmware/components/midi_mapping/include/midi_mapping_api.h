#pragma once

#include "audio_param.h"
#include "etl_utility.h"
#include "midi_mapping.h"
#include <variant>
#include <utility>

namespace shrapnel {
namespace midi {

struct GetRequest {};

struct GetResponse {
    //etl::imap mappings,
    std::strong_ordering operator<=>(const GetResponse &other) const = default;
};

struct CreateRequest {
    std::pair<Mapping::id_t, Mapping> mapping;
    std::strong_ordering operator<=>(const CreateRequest &other) const = default;

    // TODO prefer free function STL style over tightly coupled members
    friend etl::string_stream& operator<<(etl::string_stream&  out, const CreateRequest& self) {
        out << "{ " << self.mapping << " }";
        return out;
    }
};

struct CreateResponse {
    std::pair<Mapping::id_t, Mapping> mapping;
    std::strong_ordering operator<=>(const CreateResponse &other) const = default;
};

struct Update {
    std::pair<Mapping::id_t, Mapping> mapping;
    std::strong_ordering operator<=>(const Update &other) const = default;
};

struct Remove {
    Mapping::id_t id;
    std::strong_ordering operator<=>(const Remove &other) const = default;
};

using MappingApiMessage = std::variant<GetRequest, GetResponse, CreateRequest, CreateResponse, Update, Remove>;

}
}
