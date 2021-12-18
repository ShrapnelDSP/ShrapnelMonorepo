#pragma once

namespace shrapnel {

typedef void (*task_func_t)(void *context);

template<task_func_t init_function, task_func_t work_function>
class Task//<init_function, work_function>
{
    public:
    Task(const char *name, size_t stack, int priority, void *context);

    private:
    void *context;
};

};
