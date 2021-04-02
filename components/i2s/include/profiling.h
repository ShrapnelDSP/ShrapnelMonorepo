#pragma once

void profiling_start(void);
void profiling_mark_stage(unsigned int stage);

void i2s_profiling_task(void *param);
