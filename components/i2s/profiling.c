#include "i2s.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <assert.h>

#define TAG "profiling"

#define NUMBER_OF_STAGES 15

static int64_t start_time = 0;
static int64_t stage_time[NUMBER_OF_STAGES] = {};

void profiling_start(void)
{
    start_time = esp_timer_get_time();
}

void profiling_mark_stage(unsigned int stage)
{
    assert(stage < NUMBER_OF_STAGES);

    stage_time[stage] = esp_timer_get_time();
}

static float time_to_percent(int64_t time)
{
    //TODO expose the sample rate from the i2s module
    return 100.f * time / (1e6 * DMA_BUF_SIZE / 48e3);
}

void i2s_profiling_task(void *param)
{
    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        for(int i = 0; i < NUMBER_OF_STAGES; i++)
        {
            //if the stage has not been assigned, stop printing
            if(stage_time[i] == 0)
            {
                break;
            }

            int64_t current_stage_time = start_time - stage_time[i];
            ESP_LOGI(TAG, "Stage %d processing took %lld us (%03.1f %%)",
                     i,
                     current_stage_time,
                     time_to_percent(current_stage_time));
        }
    }
}
