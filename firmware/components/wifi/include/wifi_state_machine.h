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

#include "etl/delegate.h"
#include "etl/enum_type.h"
#include "etl/state_chart.h"
#include <freertos/portmacro.h>
#include <memory>

#define WIFI_STR(x) #x
#define ETL_ENUM_TYPE_AUTO(x) ETL_ENUM_TYPE(x, WIFI_STR(x))

namespace shrapnel {
namespace wifi {

struct InternalEvent
{
    enum enum_type
    {
        IS_PROVISIONED,
        IS_NOT_PROVISIONED,
        STARTED,
        CONNECT_SUCCESS,
        CONNECT_TIMEOUT,
        DISCONNECT,
        RESET_PROVISIONING,
        PROVISIONING_SUCCESS,
        PROVISIONING_FAILURE,
        PROVISIONING_BYPASS,
        PROVISIONING_END,
    };

    ETL_DECLARE_ENUM_TYPE(InternalEvent, int)
    ETL_ENUM_TYPE_AUTO(IS_PROVISIONED)
    ETL_ENUM_TYPE_AUTO(IS_NOT_PROVISIONED)
    ETL_ENUM_TYPE_AUTO(STARTED)
    ETL_ENUM_TYPE_AUTO(CONNECT_SUCCESS)
    ETL_ENUM_TYPE_AUTO(CONNECT_TIMEOUT)
    ETL_ENUM_TYPE_AUTO(DISCONNECT)
    ETL_ENUM_TYPE_AUTO(RESET_PROVISIONING)
    ETL_ENUM_TYPE_AUTO(PROVISIONING_SUCCESS)
    ETL_ENUM_TYPE_AUTO(PROVISIONING_FAILURE)
    ETL_ENUM_TYPE_AUTO(PROVISIONING_BYPASS)
    ETL_ENUM_TYPE_AUTO(PROVISIONING_END)
    ETL_END_ENUM_TYPE
};

struct UserEvent
{
    enum enum_type
    {
        CONNECTED,
        DISCONNECTED,
    };

    ETL_DECLARE_ENUM_TYPE(UserEvent, int)
    ETL_ENUM_TYPE_AUTO(CONNECTED)
    ETL_ENUM_TYPE_AUTO(DISCONNECTED)
    ETL_END_ENUM_TYPE
};

struct State
{
    enum enum_type
    {
        INIT,
        STARTING,
        CONNECTING,
        CONNECTED,
        PROVISIONING,
        PROVISIONING_STOPPING,
        PROVISIONING_FINISHING,
    };

    ETL_DECLARE_ENUM_TYPE(State, int)
    ETL_ENUM_TYPE(INIT, "INIT")
    ETL_ENUM_TYPE(STARTING, "STARTING")
    ETL_ENUM_TYPE(CONNECTING, "CONNECTING")
    ETL_ENUM_TYPE(CONNECTED, "CONNECTED")
    ETL_ENUM_TYPE(PROVISIONING, "PROVISIONING")
    ETL_ENUM_TYPE(PROVISIONING_STOPPING, "PROVISIONING_STOPPING")
    ETL_ENUM_TYPE(PROVISIONING_FINISHING, "PROVISIONING_FINISHING")
    ETL_END_ENUM_TYPE
};

/**
 * \startuml
 * title Wifi State Machine
 *
 * [*] --> INIT
 * INIT -r-> STARTING: IS_PROVISIONED
 * INIT --> PROVISIONING: IS_NOT_PROVISIONED
 *
 * STARTING --> CONNECTING: STARTED
 *
 * CONNECTING --> CONNECTING: DISCONNECT
 * CONNECTING --> CONNECTED: CONNECT_SUCCESS
 * CONNECTING --> PROVISIONING: CONNECT_TIMEOUT
 *
 * CONNECTED --> CONNECTING: DISCONNECT
 * CONNECTED --> PROVISIONING: RESET_PROVISIONING
 *
 * PROVISIONING --> PROVISIONING_FINISHING: PROVISIONING_SUCCESS
 * PROVISIONING --> PROVISIONING_STOPPING: PROVISIONING_FAILURE
 *
 * PROVISIONING_FINISHING --> CONNECTED: PROVISIONING_END
 *
 * PROVISIONING_STOPPING --> PROVISIONING: PROVISIONING_END
 *
 * \enduml
 */
class WifiStateMachine
{
public:
    using internal_event_callback_t = etl::delegate<void(InternalEvent)>;
    using user_event_callback_t = etl::delegate<void(UserEvent)>;

    WifiStateMachine(internal_event_callback_t a_send_event_internal,
                     user_event_callback_t a_send_event_user);

    using transition = etl::state_chart_traits::transition<WifiStateMachine>;
    using state = etl::state_chart_traits::state<WifiStateMachine>;

    static const transition transition_table[12];
    static const state state_table[7];

private:
    void check_if_provisioned();
    void provisioning_init();
    void provisioning_start();
    void provisioning_stop();
    void provisioning_deinit();
    void start();
    void connect_with_timeout();
    void connect_to_ap();
    bool is_reconnecting();
    void on_connected();
    void on_disconnected();

    internal_event_callback_t send_event_internal;
    user_event_callback_t send_event_user;

    TickType_t connect_start_tick_count = 0;
};

} // namespace wifi
} // namespace shrapnel

#undef WIFI_STR