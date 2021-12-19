#pragma once

#include "queue.h"
#include "audio_param.h"

typedef struct
{
    char json[128];
} cmd_message_t;

void cmd_init(shrapnel::Queue<cmd_message_t> *q, shrapnel::AudioParameters *param);
void cmd_task_work(void *context);
