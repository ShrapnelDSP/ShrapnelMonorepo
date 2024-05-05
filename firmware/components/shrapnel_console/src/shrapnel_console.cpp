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
#include "midi_mapping_api.h"
#include <argtable3/argtable3.h>
#include <cassert>
#include <cppcodec/base64_rfc4648.hpp>
#include <cstdio>
#include <cstring>
#include <esp_console.h>
#include <span>

#define TAG "shrapnel_console"

namespace shrapnel {

// esp_console doesn't allow passing user pointer to command handlers. We make
// the console a singleton, and save its instance in this scope so that command
// callbacks can read it.
static Console *instance;

static struct
{
    struct arg_str *data;
    struct arg_end *end;
} arg_midi;

int handle_midi(int argc, char *argv[])
{
    ESP_LOGI(TAG, "midi argc %d", argc);

    for(int i = 0; i < argc; i++)
    {
        ESP_LOGI(TAG, "midi argv %d: %s", i, argv[i]);
    }

    int nerrors = arg_parse(argc, argv, (void **)(&arg_midi));
    if(nerrors != 0)
    {
        char error_message_memory[1024] = {};
        auto file =
            fmemopen(error_message_memory, sizeof error_message_memory, "w");
        arg_print_errors(file, arg_midi.end, "midi");

        ESP_LOGE(TAG, "%s", error_message_memory);
        return -1;
    }

    ESP_LOGI(TAG, "data: %s", arg_midi.data->sval[0]);

    std::array<uint8_t, 128> buffer{};
    size_t decoded_size = 0;
    auto error =
        cppcodec::base64_rfc4648::decode(buffer.data(),
                                         buffer.size(),
                                         arg_midi.data->sval[0],
                                         strlen(arg_midi.data->sval[0]),
                                         decoded_size);

    if(!holds_alternative<std::monostate>(error))
    {
        ESP_LOGE(TAG, "base64 decode error");
        return -1;
    }

    ESP_LOG_BUFFER_HEX_LEVEL(TAG, buffer.data(), decoded_size, ESP_LOG_INFO);

    auto message =
        api::from_bytes<midi::Message>(std::span(buffer.data(), decoded_size));
    if(!message.has_value())
    {
        ESP_LOGE(TAG, "failed to decode MIDI message");
        return -1;
    }

    etl::string<64> string;
    etl::string_stream stream(string);

    stream << message.value();

    ESP_LOGI(TAG, "decoded %s", string.c_str());

    instance->midi_message_callback(message.value());

    return 0;
}

Console::Console(MidiMessageSendCallback a_midi_message_callback)
    : midi_message_callback{a_midi_message_callback}
{
    assert(instance == nullptr);
    instance = this;

    arg_midi.data =
        arg_str1("d", "data", "<base64>", "MIDI message protobuf as base64");
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
        .help = "Inject a fake midi message for testing",
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