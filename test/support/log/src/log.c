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

#include "esp_log.h"
#include <stdio.h>
#include <stdarg.h>

uint32_t esp_log_timestamp(void)
{
    return 0;
}

static void esp_log_writev(esp_log_level_t level,
                   const char *tag,
                   const char *format,
                   va_list args)
{
    (void) level;
    (void) tag;

    vprintf(format, args);
}

void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...)
{
    va_list list;
    va_start(list, format);
    esp_log_writev(level, tag, format, list);
    va_end(list);
}
