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

#include "etl/string.h"
#include "etl/string_view.h"

namespace shrapnel::persistence {

/** Interface for storing persistent data
 *
 * The data sent to \ref save should be reloaded by \ref load even after power
 * down.
 */
class Storage
{
public:
    /// \return  non-zero on error
    [[nodiscard]] virtual int save(const char *key, etl::string_view data) = 0;
    [[nodiscard]] virtual int save(const char *key, uint32_t data) = 0;
    /// \return  non-zero on error
    [[nodiscard]] virtual int load(const char *key, etl::istring &data) = 0;
    [[nodiscard]] virtual int load(const char *key, uint32_t &data) = 0;
};

} // namespace shrapnel::persistence