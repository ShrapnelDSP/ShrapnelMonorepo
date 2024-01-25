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

#pragma once

#include "persistence.h"
#include "presets.h"

namespace shrapnel::selected_preset {

class SelectedPresetManager
{
public:
    explicit SelectedPresetManager(
        std::shared_ptr<persistence::Storage> storage);

    [[nodiscard]] int set(presets::id_t id);
    [[nodiscard]] int get(presets::id_t &id);

private:
    std::shared_ptr<persistence::Storage> storage;
};

} // namespace shrapnel::selected_preset