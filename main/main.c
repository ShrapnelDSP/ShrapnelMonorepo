#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2s.h"

void app_main(void)
{
    i2s_setup();
    printf("setup done\n");
}
