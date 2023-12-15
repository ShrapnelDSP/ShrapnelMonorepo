#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef int esp_err_t;

static inline const char *esp_err_to_name(esp_err_t) { return ""; }

#ifdef __cplusplus
} // extern "C"
#endif

#define ESP_OK 0