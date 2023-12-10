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

#include "selected_preset_manager.h"

#include <utility>

namespace shrapnel::selected_preset {

constexpr char key[] = "selected_preset";

SelectedPresetManager::SelectedPresetManager(
    std::shared_ptr<persistence::Storage> storage)
    : storage{std::move(storage)}
{
}

int SelectedPresetManager::set(presets::id_t id)
{
    int rc = storage->save(key, id);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

int SelectedPresetManager::get(id_t &id)
{
    int rc = storage->load(key, id);
    if(rc != 0)
    {
        return -1;
    }

    return 0;
}

} // namespace shrapnel::selected_preset