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

#pragma once

#include "audio_param.h"
#include "cmd_handling_api.h"
#include "esp_log.h"
#include "etl/list.h"
#include <climits>
#include <etl/delegate.h>
#include <iterator>
#include <memory>
#include <string.h>
#include <string_view>

namespace shrapnel::parameters {

template <typename AudioParametersT>
class CommandHandling final
{
public:
    using SendMessageCallback =
        etl::delegate<void(const ApiMessage &, std::optional<int>)>;

    /** \brief
     *
     * \param[in] a_param Data received through \ref dispatch() is
     * translated to binary and sent to this object.
     */
    CommandHandling(std::shared_ptr<AudioParametersT> a_param,
                    SendMessageCallback a_send_message)
        : param(a_param),
          send_message(a_send_message)
    {
    }

    void dispatch(const ApiMessage &a_message, int fd)
    {
        std::visit(
            [&](const auto &message) -> void
            {
                using T = std::decay_t<decltype(message)>;

                if constexpr(std::is_same_v<T, Update>)
                {
                    parameter_update(message, fd);
                }
                else if constexpr(std::is_same_v<T, Initialise>)
                {
                    initialise_parameters();
                }
                else
                {
                    ESP_LOGE(TAG, "Unhandled message type");
                }
            },
            a_message);
    }

private:
    void parameter_update(const Update &message, int fd)
    {
        ESP_LOGD(TAG,
                 "parameter update %s %f %d",
                 message.id.data(),
                 message.value,
                 fd);

        int rc = param->update(message.id, message.value);
        if(rc != 0)
        {
            ESP_LOGE(TAG,
                     "Failed to update parameter (%s) with value %f",
                     message.id.data(),
                     message.value);
        }

        send_message(message, fd);
    }

    void initialise_parameters()
    {
        ESP_LOGD(TAG, "initialise");

        for(const auto &[key, value] : *param)
        {
            Update message = {
                .id{id_t{key}},
                .value{value->get()},
            };

            send_message(message, std::nullopt);
        }
    }

    std::shared_ptr<AudioParametersT> param;
    SendMessageCallback send_message;

    static inline const char *TAG = "cmd_handling";
};

} // namespace shrapnel::parameters
