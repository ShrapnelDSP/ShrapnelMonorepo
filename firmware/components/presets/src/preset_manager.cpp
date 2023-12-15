/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#include "presets_manager.h"

#define TAG "preset_manager"

namespace shrapnel::presets {

int PresetsManager::create(const PresetData &preset, id_t &id_out)
{
    std::array<uint8_t, 100> buffer;
    auto data = std::span<uint8_t, std::dynamic_extent>(buffer);

    int rc = serialise_preset(preset, data);
    if(rc != 0)
    {
        return -1;
    }

    rc = storage->create(data, id_out);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

std::optional<PresetData> PresetsManager::read(id_t id)
{
    std::array<uint8_t, 100> buffer;
    auto data = std::span<uint8_t, std::dynamic_extent>(buffer);
    int rc = storage->load(id, data);
    if(rc != 0)
    {
        return std::nullopt;
    }

    PresetData preset;
    rc = deserialise_preset(data, preset);
    if(rc != 0)
    {
        return std::nullopt;
    }

    return preset;
}

int PresetsManager::update(id_t id, const PresetData &preset)
{
    std::array<uint8_t, 100> buffer;
    auto data = std::span<uint8_t, std::dynamic_extent>(buffer);

    int rc = serialise_preset(preset, data);
    if(rc != 0)
    {
        return -1;
    }

    rc = storage->save(id, data);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

int PresetsManager::remove(id_t id)
{
    int rc = storage->remove(id);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

void PresetsManager::for_each(
    etl::delegate<void(id_t, const PresetData &)> callback)
{
    for(const auto &entry : *storage)
    {
        auto id = key_to_id(entry.key);
        auto preset = read(id);
        if(!preset.has_value())
        {
            ESP_LOGE(TAG, "Failed to read preset at key %s", entry.key);
            continue;
        }
        callback(id, *preset);
    }
}

} // namespace shrapnel::presets