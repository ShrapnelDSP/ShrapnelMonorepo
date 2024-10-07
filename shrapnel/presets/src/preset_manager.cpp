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

PresetsManager::PresetsManager(
    std::unique_ptr<persistence::Crud<std::span<uint8_t>>> a_storage)
    : storage{std::move(a_storage)}
{
}

int PresetsManager::create(const PresetData &preset, id_t &id_out)
{
    std::array<uint8_t, 100> buffer;
    auto data = std::span<uint8_t, std::dynamic_extent>(buffer);

    auto encoded = api::to_bytes(preset, data);
    if(!encoded.has_value())
    {
        return -1;
    }

    int rc = storage->create(*encoded, id_out);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

int PresetsManager::read(id_t id, PresetData &preset)
{
    std::array<uint8_t, 100> buffer;
    auto data = std::span<uint8_t>(buffer);
    int rc = storage->read(id, data);
    if(rc != 0)
    {
        return -1;
    }

    auto decoded = api::from_bytes<PresetData>(data);
    if(!decoded.has_value())
    {
        return -1;
    }

    preset = *decoded;

    return 0;
}

int PresetsManager::update(id_t id, const PresetData &preset)
{
    std::array<uint8_t, 100> buffer;
    auto data = std::span<uint8_t, std::dynamic_extent>(buffer);

    auto encoded = api::to_bytes(preset, data);
    if(!encoded.has_value())
    {
        return -1;
    }

    int rc = storage->update(id, *encoded);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

int PresetsManager::destroy(id_t id)
{
    int rc = storage->destroy(id);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

void PresetsManager::for_each(
    etl::delegate<void(id_t, const PresetData &)> callback)
{
    storage->for_each(
        [&callback](uint32_t id, std::span<uint8_t> data)
        {
            auto decoded = api::from_bytes<PresetData>(data);
            if(!decoded.has_value())
            {
                return;
            }

            callback(id, *decoded);
        });
}

} // namespace shrapnel::presets