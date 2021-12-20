#include "task.h"

#include <cassert>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace shrapnel {

TaskBase::TaskBase(const char *name, size_t stack, int priority) :
    name(name),
    stack(stack),
    priority(priority) {}

void TaskBase::task_thread(void *param)
{
    TaskBase *task = static_cast<TaskBase *>(param);

    task->setup();
    while (1)
    {
        task->loop();
    }
}

void TaskBase::setup(void)
{
}

void TaskBase::start(void)
{
    int rc = xTaskCreate(task_thread, name, stack, this, priority, &handle);
    assert(rc == pdPASS);
}

TaskBase::~TaskBase(void)
{
    vTaskDelete(handle);
}

}
