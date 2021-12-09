#include "audio_events.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define TAG "audio_events"

EventGroupHandle_t g_audio_event_group;

static QueueHandle_t out_queue;

static const char *clipping_message = "{\"event\": \"Output Clipped\"}";

static void websocket_send(void *arg)
{
    (void) arg;

    char buffer[1024 + 1] = {0};

    xQueueReceive(out_queue, buffer, 100 / portTICK_PERIOD_MS);

    httpd_ws_frame_t pkt = {
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = (void *)buffer,
        .len = strlen(buffer),
    }

    /* TODO how do we get the current server's handle? It's already used when
     * we queue work, maybe just pass it as arg? */


    httpd_get_client_list(httpd_handle_t handle, size_t *fds, int *client_fds);

    /* TODO send the message to all clients */

}

#define CLIPPING_REPEAT_TICK (1000/portTICK_PERIOD_MS)
static void audio_event_task(void *parameters)
{
    int bits;
    TickType_t last_clipping_tick = 0;

    while(1)
    {
        bits = xEventGroupWaitBits(g_audio_event_group, 0xFF, true, false, portMAX_DELAY);

        if(bits & AUDIO_EVENT_OUTPUT_CLIPPED)
        {

            /* Clipping messages are throttled to not send more data then
             * needed */
            if((xTaskGetTickCount() - last_clipping_tick) > CLIPPING_REPEAT_TICK)
            {
                // send a message to make the web UI show the clipping
                if(errQUEUE_FULL == xQueueSendToBack(out_queue, &clipping_message, 100 / portTICK_PERIOD_MS))
                {
                    ESP_LOGE(TAG, "Failed to send clipping message to websocket");
                }

                /* TODO expose server */
                esp_err_t rc = httpd_queue_work(server, websocket_send, NULL);

                ESP_LOGI(TAG, "Sent clipping message");
                last_clipping_tick = xTaskGetTickCount();
            }
            //clear the bit we just checked
            bits &= (~AUDIO_EVENT_OUTPUT_CLIPPED);
        }

        if(bits)
        {
            ESP_LOGE(TAG, "Some bits were not handled %x", bits);
        }
    }
}

esp_err_t audio_event_init(QueueHandle_t q)
{
    out_queue = q;

    g_audio_event_group = xEventGroupCreate();

    if(g_audio_event_group == NULL)
    {
        ESP_LOGE(TAG, "Failed to create event group");
        return ESP_FAIL;
    }

    if(pdPASS != xTaskCreate(audio_event_task, "audio event", 2000, NULL, 4, NULL))
    {
        ESP_LOGE(TAG, "Failed to create task");
        return ESP_FAIL;
    }

    return ESP_OK;
}
