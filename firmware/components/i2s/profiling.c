#include "profiling.h"

#include "i2s.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/task.h"
#include <assert.h>

#define TAG "profiling"

#define NUMBER_OF_STAGES 15

static int64_t start_time = 0;
static int64_t stage_time[NUMBER_OF_STAGES] = {};
SemaphoreHandle_t profiling_mutex;
static bool got_semaphore = false;

void profiling_start(void)
{
    if(xSemaphoreTake(profiling_mutex, 0))
    {
        got_semaphore = true;
        start_time = esp_timer_get_time();
    }
}

void profiling_mark_stage(unsigned int stage)
{
    assert(stage < NUMBER_OF_STAGES);

    if(got_semaphore)
    {
        stage_time[stage] = esp_timer_get_time();
    }
}

void profiling_stop(void)
{
    if(got_semaphore)
    {
        xSemaphoreGive(profiling_mutex);
        got_semaphore = false;
    }
}

static double time_to_percent(int64_t time)
{
    //TODO expose the sample rate from the i2s module
    return 100 * time / (1e6 * DMA_BUF_SIZE / 48e3);
}

void i2s_profiling_task(void *param)
{
    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        if(xSemaphoreTake(profiling_mutex, 100 / portTICK_PERIOD_MS))
        {
            ESP_LOGI(TAG, " ========= \n");

            int64_t total_time = 0;
            for(int i = 0; i < NUMBER_OF_STAGES; i++)
            {
                //if the stage has not been assigned, stop printing
                if(stage_time[i] == 0)
                {
                    break;
                }

                int64_t current_stage_time = stage_time[i] - ((i == 0) ? start_time : stage_time[i - 1]);
                ESP_LOGI(TAG, "Stage %d processing took %lld us (%03.1f %%)",
                        i,
                        current_stage_time,
                        time_to_percent(current_stage_time));

                total_time += current_stage_time;
            }

            ESP_LOGI(TAG, "Total processing took %lld us (%03.1f %%)",
                    total_time,
                    time_to_percent(total_time));

            ESP_LOGI(TAG, " ========= \n");

            xSemaphoreGive(profiling_mutex);
        }
        else
        {
            ESP_LOGE(TAG, "%s failed to take semaphore", __FUNCTION__);
        }
    }
}
