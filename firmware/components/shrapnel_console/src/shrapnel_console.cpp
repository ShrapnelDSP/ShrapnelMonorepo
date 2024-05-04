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
#include <argtable3/argtable3.h>
#include <cassert>
#include <cstdio>
#include <esp_console.h>

#define TAG "shrapnel_console"

namespace shrapnel {

// esp_console doesn't allow passing user pointer to command handlers. We make
// the console a singleton, and save its instance in this scope so that command
// callbacks can read it.
static Console *instance;

static struct
{
    struct arg_int *data;
    struct arg_end *end;
} arg_midi;

static int handle_midi(int argc, char *argv[])
{
    ESP_LOGI(TAG, "midi argc %d", argc);

    for(int i = 0; i < argc; i++)
    {
        ESP_LOGI(TAG, "midi argv %d: %s", i, argv[i]);
    }

    int nerrors = arg_parse(argc, argv, (void **)(&arg_midi));

    ESP_LOGW(TAG, "nerrors = %d", nerrors);

    if(nerrors == 0)
    {
        int count = arg_midi.data->count;
        for(int i = 0; i < count; i++)
        {
            ESP_LOGI(TAG, "data: %d", arg_midi.data->ival[i]);
        }
    }
    else
    {
        char error_message_memory[1024] = {};
        auto file =
            fmemopen(error_message_memory, sizeof error_message_memory, "w");
        arg_print_errors(file, arg_midi.end, "midi");

        ESP_LOGE(TAG, "%s", error_message_memory);
        return -1;
    }

    // TODO parse, handle request
    return 0;
}

Console::Console()
{
    assert(instance == nullptr);
    instance = this;

    arg_midi.data = arg_intn("b", "byte", nullptr, 1, 3, "MIDI bytes");
    arg_midi.end = arg_end(0);

    embedded_cli_init(&cli, "shrapnel $ ", Console::putch, this);
    esp_console_config_t console_config{
        .max_cmdline_length = EMBEDDED_CLI_MAX_LINE,
        .max_cmdline_args = EMBEDDED_CLI_MAX_ARGC,
        .hint_color = '0',
        .hint_bold = false,
    };

    esp_console_init(&console_config);
    esp_console_register_help_command();

    esp_console_cmd_t midi_command{
        .command = "midi",
        .help = "Inject a fake midi command for testing",
        .func = handle_midi,
        .argtable = &arg_midi,
    };

    esp_console_cmd_register(&midi_command);
} // namespace shrapnel

void Console::handle_character(char c)
{
    auto needs_processing = embedded_cli_insert_char(&cli, c);
    if(needs_processing)
    {
        const char *line = embedded_cli_get_line(&cli);
        assert(line != nullptr);

        int rc = 0;
        int err = esp_console_run(line, &rc);
        if(err != ESP_OK)
        {
            ESP_LOGE(
                TAG, "esp_console_run failed %d %s", err, esp_err_to_name(err));
            embedded_cli_prompt(&cli);
            return;
        }

        if(rc != 0)
        {
            ESP_LOGE(TAG, "command returned %d", rc);
        }

        embedded_cli_prompt(&cli);
    }
}

void Console::putch(void *, char c, bool) { putc(c, stdout); }
} // namespace shrapnel