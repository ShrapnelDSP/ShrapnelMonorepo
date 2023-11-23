#pragma once

#include <etl/string_stream.h>
#include <variant>

#include "presets.h"

namespace shrapnel::presets {

struct Read
{
};

struct Notify
{
    id_t selectedPresetId;
};

struct Write
{
    id_t selectedPresetId;
};

using SelectedPresetApiMessage = std::variant<Read, Notify, Write>;

etl::string_stream &operator<<(etl::string_stream &out, const Read &self);
etl::string_stream &operator<<(etl::string_stream &out, const Notify &self);
etl::string_stream &operator<<(etl::string_stream &out, const Write &self);

} // namespace shrapnel::presets