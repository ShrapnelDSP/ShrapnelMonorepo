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

#include "crud.h"
#include <cstdint>
#include <span>

namespace shrapnel {

class MockStorage final : public persistence::Crud<std::span<uint8_t>>
{
public:
    [[nodiscard]] int create(const std::span<uint8_t> &data,
                             uint32_t &id_out) override
    {
        id_out = next_key;
        assert(!storage.contains(id_out));
        storage[id_out].assign(data.begin(), data.end());
        next_key++;
        return 0;
    };

    [[nodiscard]] int read(uint32_t id, std::span<uint8_t> &data_out) override
    {
        const auto &entry = storage[id];
        if(data_out.size() < entry.size())
        {
            return -1;
        }

        std::copy(entry.begin(), entry.end(), data_out.begin());
        data_out = data_out.subspan(entry.size());
        return 0;
    };

    [[nodiscard]] int update(uint32_t id,
                             const std::span<uint8_t> &data) override
    {
        storage[id].assign(data.begin(), data.end());
        return 0;
    };

    [[nodiscard]] int destroy(uint32_t id) override
    {
        storage.erase(id);
        return 0;
    };

    void for_each(etl::delegate<void(uint32_t, const std::span<uint8_t> &)>
                      callback) override
    {
        for(const auto &[id, entry] : storage)
        {
            std::vector<uint8_t> copy{entry};
            std::span<uint8_t> span{copy};
            callback(id, span);
        }
    };

private:
    uint32_t next_key = 0;
    std::map<uint32_t, std::vector<uint8_t>> storage;
};

} // namespace shrapnel