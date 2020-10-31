#include <stdio.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#define TAG "main"

#include "i2s.h"

static void i2s_gain_task(void *param)
{
    bool b = 0;
    int att;
    float gain;
    while(1)
    {
        b = !b;
        att = b ? 0 : 6;
        gain = powf(10.f, -att / 20.f);
        ESP_LOGI(TAG, "attenuation is: %d dB", att);
        ESP_LOGI(TAG, "gain is: %01.3f", gain);

        i2s_set_gain(gain);
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    int ret;
    i2s_setup();

    ret = xTaskCreate(i2s_gain_task, "i2s gain", 2000, NULL, 5, NULL);
    if(ret != pdPASS)
    {
        ESP_LOGE(TAG, "Gain task create failed %d", ret);
    }

    printf("setup done\n");
}
