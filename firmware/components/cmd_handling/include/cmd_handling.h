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
#include "cmd_handling_api.h"
#include "cmd_handling_json.h"
#include "etl/list.h"
#include "event_send.h"
#include "esp_log.h"
#include <climits>
#include "esp_err.h"
#include "cmd_handling_json_builder.h"
#include <memory>
#include <string.h>
#include <iterator>
#include <string_view>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace shrapnel::parameters {

template<typename AudioParametersT, typename EventSendT>
class CommandHandling final
{
    public:
    /** \brief
     *
     * \param[in] a_param Data received through \ref dispatch() is
     * translated to binary and sent to this object.
     */
    CommandHandling(
            AudioParametersT *a_param,
            EventSendT &a_event) :
        param(a_param),
        event(a_event)
        {}

    void dispatch(const ApiMessage &message, int fd)
    {
        // TODO refactor so that this parsing is done in the server thread.
        // This function should take an ApiMessage
#if !defined(TESTING)
        ESP_LOGI(TAG, "%s stack %d", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL));
#endif

        std::visit([&](const auto &message) -> void {
            using T = std::decay_t<decltype(message)>;

            if constexpr (std::is_same_v<T, Update>) {
                parameter_update(message, fd);
            } else if constexpr (std::is_same_v<T, Initialise>) {
                initialise_parameters();
            } else {
                ESP_LOGE(TAG, "Unhandled message type");
            }
        }, message);
    }

    private:
    void parameter_update(const Update &message, int fd)
    {
        int rc = param->update(message.id, message.value);
        if(rc != 0)
        {
            ESP_LOGE(TAG, "Failed to update parameter (%s) with value %f", message.id.data(), message.value);
        }

        event.send(message, fd);
    }

    void initialise_parameters()
    {
        for(const auto& [key, value] : *param)
        {
            Update message = {
                    .id{id_t{key}},
                    .value{value->get()},
            };

            event.send(message, -1);
        }
    }

    AudioParametersT *param;
    EventSendT &event;

    static inline const char *TAG = "cmd_handling";
};

}
