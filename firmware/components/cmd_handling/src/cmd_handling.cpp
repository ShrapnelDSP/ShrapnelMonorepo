#include "cmd_handling.h"

#include "audio_param.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include <memory>
#include <string.h>
#include "task.h"
#include "queue.h"

#define TAG "cmd_handling"

static shrapnel::Queue<char[128]> * in_queue;
static shrapnel::AudioParameters *parameters;

static audio_param_t get_id_for_param(const char *name)
{
    typedef struct {
        const char *name;
        audio_param_t id;
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
        {NULL, PARAM_MAX},
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
    char s[128 + 1];
    memset(s, 0, sizeof(s));

    /* TODO should not leave these uninitialised */
    cJSON *json;

    cJSON *id;
    audio_param_t parsed_id;

    cJSON *value;
    float parsed_value;

    int ret = in_queue->receive(s, portMAX_DELAY);
    if(ret == pdTRUE)
    {
#if !defined(TESTING)
        ESP_LOGI(TAG, "%s stack %d", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL));
#endif
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
            parameters->update(parsed_id, parsed_value);
        }
done:
        cJSON_Delete(json);
    }
    else
    {
        ESP_LOGE(TAG, "Queue failed to receive");
    }
}

void cmd_init(shrapnel::Queue<char[128]> *q, shrapnel::AudioParameters *param)
{
    in_queue = q;
    parameters = param;

    // TODO how to get the mock in here?
    //      Pass in a task factory that can be mocked?
    // shrapnel::Task<nullptr, cmd_task_work>("command task", 4000, 5, NULL);
}
