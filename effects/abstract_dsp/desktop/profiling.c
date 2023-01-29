#include "profiling.h"

void profiling_init(size_t buffer_size, float sample_rate) {
    (void) buffer_size;
    (void) sample_rate;
}
void profiling_start(void) {}
void profiling_mark_stage(unsigned int stage) {
    (void) stage;
}
void profiling_stop(void) {}

void profiling_task(void *param) {
    (void) param;
}
