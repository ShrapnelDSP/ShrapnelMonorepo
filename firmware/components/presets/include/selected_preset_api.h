#pragma once

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

} // namespace shrapnel::presets