#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

extern SemaphoreHandle_t profiling_mutex;

void profiling_start(void);
void profiling_mark_stage(unsigned int stage);
void profiling_stop(void);

void i2s_profiling_task(void *param);

#ifdef __cplusplus
}
#endif
