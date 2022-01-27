#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace shrapnel {

class TaskBase
{
    public:
    TaskBase(const char *name, size_t stack, int priority);
    ~TaskBase(void);

    protected:
    /** Called once when the task starts. */
    virtual void setup(void);

    /** Called continously while the task is running.
     *
     * \note The task should block in this function to avoid task starvation. */
    virtual void loop(void) = 0;

    /** Call this from the derived class' constructor as the last step to start
     * the task.
     *
     * Prevents the task running before the constructor completes.
     */
    void start(void);

    private:
    static void task_thread(void *param);

    const char *name;
    size_t stack;
    int priority;
    TaskHandle_t handle;
};

};