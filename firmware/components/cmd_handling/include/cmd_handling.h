#pragma once

#include "queue.h"
#include "audio_param.h"

void cmd_init(shrapnel::Queue<char[128]> *q, shrapnel::AudioParameters *param);
void cmd_task_work(void *context);
