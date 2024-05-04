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

extern "C" {
#include "embedded_cli.h"
}

#include <cstddef>

namespace shrapnel {
class Console
{
public:
    Console();

    void handle_character(char c);

private:
    static void putch(void *, char c, bool);

    struct embedded_cli cli;
#if 0
    char buffer[128];
    size_t buffer_used = 0;
#endif
};
} // namespace shrapnel
