/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

#include "cmd_handling.h"

#include "audio_events.h"
#include "audio_param.h"
#include "cJSON.h"
#include <climits>
#include "esp_err.h"
#include "esp_log.h"
#include <memory>
#include <string.h>
#include "task.h"
#include "queue.h"
#include <iterator>

#define TAG "cmd_handling"

namespace shrapnel {

void CommandHandling::work(void)
{
    /* TODO should not leave these uninitialised */
    char *message_type;
    cJSON *type;

    int ret = queue->receive(&message, portMAX_DELAY);
    if(ret != pdTRUE)
    {
        ESP_LOGE(TAG, "Queue failed to receive");
        return;
    }

#if !defined(TESTING)
    ESP_LOGI(TAG, "%s stack %d", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL));
#endif
    size_t message_size = sizeof(message.json);
    assert(message_size <= INT_MAX);
    (void)message_size;

    ESP_LOGI(TAG, "received websocket message: %.*s",
             static_cast<int>(sizeof(message.json)),
             message.json);

    json = cJSON_ParseWithLength(message.json, sizeof(message.json));
    if(json == NULL)
    {
        ESP_LOGE(TAG, "json parsing failed");
        goto done;
    }

    type = cJSON_GetObjectItemCaseSensitive(json, "messageType");
    if(cJSON_IsString(type) && (type->valuestring != NULL))
    {
        message_type = type->valuestring;
    }
    else
    {
        ESP_LOGE(TAG, "error parsing messageType from json");
        goto done;
    }

    if(0 == strcmp(message_type, "initialiseParameters"))
    {
        initialise_parameters();
    }
    else if(0 == strcmp(message_type, "parameterUpdate"))
    {
        parameter_update();
    }
    else
    {
        ESP_LOGE(TAG, "unknown message type (%s)", message_type);
    }

done:
    cJSON_Delete(json);
}

void CommandHandling::parameter_update(void)
{
    assert(json);

    char *parsed_id;
    cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
    if(cJSON_IsString(id) && (id->valuestring != NULL))
    {
        parsed_id = id->valuestring;
    }
    else
    {
        ESP_LOGE(TAG, "error parsing id from json");
        return;
    }

    float parsed_value;
    cJSON *value = cJSON_GetObjectItemCaseSensitive(json, "value");
    if(cJSON_IsNumber(value))
    {
        parsed_value = value->valuedouble;
    }
    else
    {
        ESP_LOGE(TAG, "error parsing value from json");
        return;
    }

    int rc = param->update(parsed_id, parsed_value);
    if(rc != 0)
    {
        ESP_LOGE(TAG, "Failed to update parameter (%s) with value %f", parsed_id, parsed_value);
    }

    audio_event_send_callback(message.json, message.fd);
}

void CommandHandling::initialise_parameters(void)
{
    assert(json);

    Message output;

    for(const auto& [key, value] : *param)
    {
        float tmp_f = *value->get_raw_parameter();

        snprintf(output.json, sizeof(output.json),
                 "{\"id\": \"%s\", \"value\":%g}",
                 key.c_str(),
                 tmp_f);

        audio_event_send_callback(output.json, -1);
    }
}

CommandHandling::CommandHandling(
        QueueBase<Message> *queue,
        AudioParametersBase *param) :
    queue(queue),
    param(param),
    json(nullptr),
    message({}) {}

}
