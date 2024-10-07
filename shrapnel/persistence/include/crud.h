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

#include <cstdint>
#include <etl/delegate.h>
#include <optional>

namespace shrapnel::persistence {

template <typename DataT>
class Crud
{
public:
    virtual ~Crud() = default;

    [[nodiscard]] virtual int create(const DataT &data, uint32_t &id_out) = 0;
    [[nodiscard]] virtual int read(uint32_t id, DataT &data_out) = 0;
    [[nodiscard]] virtual int update(uint32_t id, const DataT &data) = 0;
    [[nodiscard]] virtual int destroy(uint32_t id) = 0;

    virtual void
    for_each(etl::delegate<void(uint32_t, const DataT &)> callback) = 0;
};

} // namespace shrapnel::persistence
