#pragma once

#include <etl/string_stream.h>
#include <variant>

#include "presets.h"
#include "uuid.h"

namespace shrapnel::selected_preset {

using shrapnel::uuid::uuid_t;

struct Read
{
};

struct Notify
{
    uuid_t selectedPresetId;
};

struct Write
{
    uuid_t selectedPresetId;
};

using SelectedPresetApiMessage = std::variant<Read, Notify, Write>;

etl::string_stream &operator<<(etl::string_stream &out, const Read &self);
etl::string_stream &operator<<(etl::string_stream &out, const Notify &self);
etl::string_stream &operator<<(etl::string_stream &out, const Write &self);

} // namespace shrapnel::presets