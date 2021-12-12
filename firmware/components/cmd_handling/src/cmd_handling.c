#include "cmd_handling.h"
#include "esp_err.h"
#include <string.h>
#include "esp_log.h"
#include "cJSON.h"
#include "audio_param.h"
#include "freertos/task.h"
#include "task.h"

#define TAG "cmd_handling"

static QueueHandle_t in_queue;
static size_t message_size;

static audio_param_t get_id_for_param(const char *name)
{
    typedef struct {
        const char *name;
        int id;
    } item_t;

    static const item_t table[] = {
        {"tight", PARAM_TIGHT},
        {"hmGain", PARAM_HM2_GAIN},
        {"ampGain", PARAM_AMP_GAIN},
        {"bass", PARAM_BASS},
        {"middle", PARAM_MIDDLE},
        {"treble", PARAM_TREBLE},
        {"volume", PARAM_VOLUME},
        {"gateThreshold", PARAM_GATE_THRESHOLD},
        {NULL, 0},
    };

    for (const item_t *p = table; p->name != NULL; ++p) {
        if (!strcmp(p->name, name)) {
            return p->id;
        }
    }

    ESP_LOGE(TAG, "Couldn't find id for parameter (%s)", name);
    return PARAM_MAX;
}

void cmd_task_work(void *context)
{
    (void) context;

    /* plus 1 size here ensures that s is always a NULL terminated string */
    char s[message_size + 1];
    memset(s, 0, sizeof(s));

    /* TODO should not leave these uninitialised */
    cJSON *json;

    cJSON *id;
    audio_param_t parsed_id;

    cJSON *value;
    float parsed_value;

    int ret = xQueueReceive(in_queue, s, portMAX_DELAY);
    if(ret == pdTRUE)
    {
        ESP_LOGI(TAG, "%s stack %d", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL));
        ESP_LOGI(TAG, "received websocket message: %s", s);

        json = cJSON_Parse(s);
        if(json == NULL)
        {
            ESP_LOGE(TAG, "json parsing failed at: %s", cJSON_GetErrorPtr());
            goto done;
        }

        id = cJSON_GetObjectItemCaseSensitive(json, "id");
        if(cJSON_IsString(id) && (id->valuestring != NULL))
        {
            parsed_id = get_id_for_param(id->valuestring);
        }
        else
        {
            ESP_LOGE(TAG, "error parsing id from json (%s)", s);
            goto done;
        }

        value = cJSON_GetObjectItemCaseSensitive(json, "value");
        if(cJSON_IsNumber(value))
        {
            parsed_value = value->valuedouble;
        }
        else
        {
            ESP_LOGE(TAG, "error parsing value from json (%s)", s);
            goto done;
        }

        if(parsed_id != PARAM_MAX)
        {
            param_update_parameter(parsed_id, parsed_value);
        }
done:
        cJSON_Delete(json);
    }
    else
    {
        ESP_LOGE(TAG, "Queue failed to receive");
    }
}

void cmd_init(QueueHandle_t q, size_t a_message_size)
{
    in_queue = q;
    message_size = a_message_size;

    static task_t task = {
        .work = cmd_task_work,
    };

    task_start(&task, "command task", 4000, 5);
}
