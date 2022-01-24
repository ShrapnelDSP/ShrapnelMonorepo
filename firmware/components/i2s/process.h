#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void process_samples(int32_t *buf, size_t buf_len);
esp_err_t process_init();

#ifdef __cplusplus
}
#endif
