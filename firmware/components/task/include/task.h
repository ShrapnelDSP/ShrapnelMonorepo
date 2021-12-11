#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef void (*task_func_t)(void *context);

typedef struct {
    task_func_t init;
    task_func_t work;
    void *context;
} task_t;

void task_start(task_t *task, const char *name, size_t stack, int priority);

#ifdef __cplusplus
}
#endif
