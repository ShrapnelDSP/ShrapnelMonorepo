#include "cmd_handling.h"

#include "audio_param.h"
#include "cJSON.h"
#include <climits>
#include "esp_err.h"
#include "esp_log.h"
#include <memory>
#include <string.h>
#include "task.h"
#include "queue.h"

#define TAG "cmd_handling"

static audio_param_t get_id_for_param(const char *name)
{
    typedef struct {
        const char *name;
        audio_param_t id;
    } item_t;

    /* TODO allow clients to register their parameters at initialisation time
     */
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

namespace shrapnel {

void CommandHandling::work(void)
{
    Message msg = {0};

    /* TODO should not leave these uninitialised */
    cJSON *json;

    cJSON *id;
    audio_param_t parsed_id;

    cJSON *value;
    float parsed_value;

    int ret = queue->receive(&msg, portMAX_DELAY);
    if(ret == pdTRUE)
    {
#if !defined(TESTING)
        ESP_LOGI(TAG, "%s stack %d", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL));
#endif
        size_t msg_size = sizeof(msg.json);
        assert(msg_size <= INT_MAX);

        ESP_LOGI(TAG, "received websocket message: %.*s",
                 static_cast<int>(sizeof(msg.json)),
                 msg.json);

        json = cJSON_ParseWithLength(msg.json, sizeof(msg.json));
        if(json == NULL)
        {
            ESP_LOGE(TAG, "json parsing failed");
            goto done;
        }

        id = cJSON_GetObjectItemCaseSensitive(json, "id");
        if(cJSON_IsString(id) && (id->valuestring != NULL))
        {
            parsed_id = get_id_for_param(id->valuestring);
        }
        else
        {
            ESP_LOGE(TAG, "error parsing id from json");
            goto done;
        }

        value = cJSON_GetObjectItemCaseSensitive(json, "value");
        if(cJSON_IsNumber(value))
        {
            parsed_value = value->valuedouble;
        }
        else
        {
            ESP_LOGE(TAG, "error parsing value from json");
            goto done;
        }

        if(parsed_id != PARAM_MAX)
        {
            param->update(parsed_id, parsed_value);
        }
done:
        cJSON_Delete(json);
    }
    else
    {
        ESP_LOGE(TAG, "Queue failed to receive");
    }
}

CommandHandling::CommandHandling(
        QueueBase<Message> *queue,
        AudioParametersBase *param) :
    queue(queue),
    param(param) {}

}
