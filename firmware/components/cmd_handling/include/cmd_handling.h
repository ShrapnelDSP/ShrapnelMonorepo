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

/**
 * \page ws_api_parameters Audio parameter JSON messages
 *
 * <table>
 * <tr><th> Message type <th> Parameters <th> Direction <th> Behaviour <th> Example
 * <tr>
 *   <td> `initialiseParameters`
 *   <td> None
 *   <td> UI -> Firmware
 *   <td> The firmware will respond by sending a `parameterUpdate` message with
 *        the current value of each audio parameter
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "initialiseParameters"
 *   }
 *   ~~~
 * <tr>
 *   <td> `parameterUpdate`
 *   <td> `id` (string): The parameter ID of the parameter to change.
 *
 *   `value` (float): The value of the parameter. This must be in the range 0 - 1.
 *   <td> Any
 *   <td> Firmware: update DSP processing to use the new parameter value.
 *
 *   Frontend: Update UI to show new parameter value.
 *   <td>
 *   ~~~
 *   {
 *     "messageType": "parameterUpdate",
 *     "id": "gain",
 *     "value": 0.5
 *   }
 *   ~~~
 * </table>
 */

#pragma once

#include "audio_param.h"
#include "etl/list.h"
#include "event_send.h"
#include "cJSON.h"
#include "esp_log.h"
#include <climits>
#include "esp_err.h"
#include <memory>
#include <string.h>
#include "task.h"
#include <iterator>
#include <string_view>

namespace shrapnel {

template<typename AudioParametersT>
class CommandHandling final
{
    public:
    struct Message
    {
        char json[256];
        int fd;
    };

    /** \brief
     *
     * \param[in] a_param Data received through \ref dispatch() is
     * translated to binary and sent to this object.
     */
    CommandHandling(
            AudioParametersT *a_param,
            EventSendBase &a_event) :
        param(a_param),
        event(a_event),
        json(nullptr)
        {}

    void dispatch(const Message &message)
    {
        /* TODO should not leave these uninitialised */
        char *message_type;
        cJSON *type;

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
            parameter_update(message);
        }
        else
        {
            ESP_LOGE(TAG, "unknown message type (%s)", message_type);
        }

done:
        cJSON_Delete(json);
    }

    private:
    void parameter_update(const Message &message)
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

        event.send(message.json, message.fd);
    }

    void initialise_parameters(void)
    {
        assert(json);

        Message output;

        for(const auto& [key, value] : *param)
        {
            float tmp_f = value->get();

            snprintf(output.json, sizeof(output.json),
                    "{\"id\":\"%s\",\"value\":%g}",
                    key.c_str(),
                    tmp_f);

            event.send(output.json, -1);
        }
    }

    AudioParametersT *param;
    EventSendBase &event;

    // TODO refactor so that JSON is not visible to this class
    cJSON *json;

    static inline const char *TAG = "cmd_handling";
};

}
