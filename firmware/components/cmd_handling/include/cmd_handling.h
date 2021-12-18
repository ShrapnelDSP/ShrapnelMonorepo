#pragma once

#include "queue.h"

void cmd_init(shrapnel::Queue<char[128]> *q);
void cmd_task_work(void *context);
