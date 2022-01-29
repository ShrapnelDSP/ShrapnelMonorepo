#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void filter_set_tight(bool t);
void filter_pedal_input_process(float *buf, size_t buf_len);
void filter_amp_input_process(float *buf, size_t buf_len);
void filter_final_process(float *buf, size_t buf_len);
esp_err_t filter_init(float sample_rate);

#ifdef __cplusplus
}
#endif
