#include "task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" {
static void task_thread(void *param);

static void task_thread(void *param)
{
    const shrapnel::task_func_t work = reinterpret_cast<shrapnel::task_func_t>(param);

    while (true) {
        work(NULL);
    }
}
}

namespace shrapnel {

template<task_func_t init_function, task_func_t work_function>
Task<init_function, work_function>::Task(const char *name, size_t stack, int priority)
{
    init_function(NULL);

    int rc = xTaskCreate(task_thread, name, stack, work_function, priority, &handle);
    assert(rc == pdPASS);
}

template<task_func_t init_function, task_func_t work_function>
Task<init_function, work_function>::~Task()
{
    vTaskDelete(handle);
}

}
