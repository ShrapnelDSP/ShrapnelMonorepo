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

#include "selected_preset_api.h"

namespace shrapnel::selected_preset {

/** Convert \p object to a JSON value
 *
 * \note The \p document must not by modified by this function. It is only
 *       passed in so that its allocator member can be accessed.
 */
template <typename T>
rapidjson::Value to_json(rapidjson::Document &document, const T &object);

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const Read &object);

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const Notify &object);

template <>
rapidjson::Value to_json(rapidjson::Document &document,
                         const Write &object);

} // namespace shrapnel::selected_preset