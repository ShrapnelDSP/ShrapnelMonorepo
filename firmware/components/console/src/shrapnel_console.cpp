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

#include "shrapnel_console.h"
#include "esp_log.h"
#include <cassert>

#define TAG "shrapnel_console"

namespace shrapnel {

Console::Console()
{
    embedded_cli_init(&cli, "shrapnel", Console::putch, this);
}

void Console::handle_character(char c)
{
    ESP_LOGI(TAG, "rx c: 0x%02x", c);
    auto needs_processing = embedded_cli_insert_char(&cli, c);
    if(needs_processing)
    {
        char **argv;
        int argc = embedded_cli_argc(&cli, &argv);
        assert(argc <= EMBEDDED_CLI_MAX_ARGC);

        for(int i = 0; i < argc; i++)
        {
            ESP_LOGI(TAG, "arg %d: %s", i, argv[i]);
        }

        embedded_cli_prompt(&cli);
    }
}

void Console::putch(void *, char c, bool) { ESP_LOGI(TAG, "tx c: 0x%02x", c); }
} // namespace shrapnel