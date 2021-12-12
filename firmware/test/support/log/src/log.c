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
    printf(format, args);
}

void esp_log_write(esp_log_level_t level, const char* tag, const char* format, ...)
{
    va_list list;
    va_start(list, format);
    esp_log_writev(level, tag, format, list);
    va_end(list);
}
