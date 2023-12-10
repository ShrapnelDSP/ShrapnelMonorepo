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

#include "preset_serialisation.h"
#include "presets_api.h"
#include "presets_storage.h"
#include <etl/delegate.h>
#include <memory>

namespace shrapnel::presets {

class PresetsManager
{
public:
    [[nodiscard]] int create(const PresetData &preset, id_t &id_out);
    [[nodiscard]] std::optional<PresetData> read(id_t id);
    [[nodiscard]] int update(id_t id, const PresetData &preset);
    [[nodiscard]] int remove(id_t id);

    void for_each(etl::delegate<void(id_t, const PresetData &)> callback);

private:
    std::unique_ptr<presets_storage::Storage> storage;
};

} // namespace shrapnel::presets