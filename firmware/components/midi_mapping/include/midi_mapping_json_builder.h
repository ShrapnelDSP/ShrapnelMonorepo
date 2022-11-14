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

#include "midi_mapping_api.h"

namespace shrapnel {
namespace midi {


// TODO Document should be const, but we can't get it's allocator when it is.
// Maybe just document that implementations must not change the document?
// Maybe pass the allocator instead of the document? Allocator can't be const either.
template<typename T>
rapidjson::Value to_json(rapidjson::Document &document, const T &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const Mapping &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const std::pair<Mapping::id_t, Mapping> &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const CreateResponse &object);

template<>
rapidjson::Value to_json(rapidjson::Document &document, const MappingApiMessage &object);

#if 0
template<>
rapidjson::Value to_json(rapidjson::Document &document, const GetResponse &object)
#endif

}
}
