#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace shrapnel {

typedef void (*task_func_t)(void *context);

template<task_func_t init_function, task_func_t work_function>
class Task
{
    public:
    Task(const char *name, size_t stack, int priority);
    ~Task();

    private:
    void *context;
    TaskHandle_t handle;
};

};
