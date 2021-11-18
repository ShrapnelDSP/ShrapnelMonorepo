#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

void process_samples(int32_t *buf, size_t buf_len);
esp_err_t process_init();
#endif //PROCESS_H
