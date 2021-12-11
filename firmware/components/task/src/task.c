#include "task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void task_thread(void *param)
{
    const task_t *task = param;

    if(task->init)
    {
        task->init(task->context);
    }

    while (true) {
        task->work(task->context);
    }
}

void task_start(task_t *task, const char *name, size_t stack, int priority)
{
    xTaskCreate(task_thread, name, stack, task, 5, NULL);
}
