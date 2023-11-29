#pragma once

#include <etl/string_stream.h>
#include <variant>

#include "presets.h"
#include "uuid.h"

namespace shrapnel::selected_preset {

using shrapnel::uuid::uuid_t;

struct Read
{

    std::strong_ordering operator<=>(const Read &other) const = default;
};

struct Notify
{
    uuid_t selectedPresetId;

    std::strong_ordering operator<=>(const Notify &other) const = default;
};

struct Write
{
    uuid_t selectedPresetId;

    std::strong_ordering operator<=>(const Write &other) const = default;
};

using SelectedPresetApiMessage = std::variant<Read, Notify, Write>;

etl::string_stream &operator<<(etl::string_stream &out, const Read &self);
etl::string_stream &operator<<(etl::string_stream &out, const Notify &self);
etl::string_stream &operator<<(etl::string_stream &out, const Write &self);
etl::string_stream &operator<<(etl::string_stream &out,
                               const SelectedPresetApiMessage &self);

} // namespace shrapnel::selected_preset