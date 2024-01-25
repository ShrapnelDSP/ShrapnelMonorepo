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

namespace shrapnel::persistence {

/** Persistent storage implemented using ESP32 nvs component
 *
 * \note Key will be truncated to 15 characters due to limitation of NVS
 */
class EspStorage final : public Storage
{
public:
    [[nodiscard]] int save(const char *key, std::span<uint8_t> data) override;
    [[nodiscard]] int save(const char *key, etl::string_view data) override;
    [[nodiscard]] int save(const char *key, uint32_t data) override;
    [[nodiscard]] virtual int save(const char *key, float data) override;

    [[nodiscard]] int load(const char *key, std::span<uint8_t> &data) override;
    [[nodiscard]] int load(const char *key, etl::istring &data) override;
    [[nodiscard]] int load(const char *key, uint32_t &data) override;
    [[nodiscard]] int load(const char *key, float &data) override;
};

} // namespace shrapnel::persistence
