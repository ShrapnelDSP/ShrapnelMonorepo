#include "etl/state_chart.h"
#include "etl/enum_type.h"
#include "wifi_provisioning.h"
#include "etl/delegate.h"
#include <memory>

namespace shrapnel {
namespace wifi {

struct InternalEvent {
    enum enum_type {
        IS_PROVISIONED,
        IS_NOT_PROVISIONED,
        STARTED,
        CONNECT_SUCCESS,
        CONNECT_TIMEOUT,
        DISCONNECT,
        RESET_PROVISIONING,
        PROVISIONING_DONE,
    };

    ETL_DECLARE_ENUM_TYPE(InternalEvent, int)
    ETL_ENUM_TYPE(IS_PROVISIONED, "IS_PROVISIONED")
    ETL_ENUM_TYPE(IS_NOT_PROVISIONED, "IS_NOT_PROVISIONED")
    ETL_ENUM_TYPE(STARTED, "STARTED")
    ETL_ENUM_TYPE(CONNECT_SUCCESS, "CONNECT_SUCCESS")
    ETL_ENUM_TYPE(CONNECT_TIMEOUT, "CONNECT_TIMEOUT")
    ETL_ENUM_TYPE(DISCONNECT, "DISCONNECT")
    ETL_ENUM_TYPE(RESET_PROVISIONING, "RESET_PROVISIONING")
    ETL_ENUM_TYPE(PROVISIONING_DONE, "PROVISIONING_DONE")
    ETL_END_ENUM_TYPE
};

struct UserEvent {
    enum enum_type {
        CONNECTED,
        DISCONNECTED,
    };

    ETL_DECLARE_ENUM_TYPE(UserEvent, int)
    ETL_ENUM_TYPE(CONNECTED, "CONNECTED")
    ETL_ENUM_TYPE(DISCONNECTED, "DISCONNECTED")
    ETL_END_ENUM_TYPE
};

struct State {
    enum enum_type {
        INIT,
        STARTING,
        CONNECTING,
        CONNECTED,
        PROVISIONING,
    };

    ETL_DECLARE_ENUM_TYPE(State, int)
    ETL_ENUM_TYPE(INIT, "INIT")
    ETL_ENUM_TYPE(STARTING, "STARTING")
    ETL_ENUM_TYPE(CONNECTING, "CONNECTING")
    ETL_ENUM_TYPE(CONNECTED, "CONNECTED")
    ETL_ENUM_TYPE(PROVISIONING, "PROVISIONING")
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
 * PROVISIONING --> CONNECTED: PROVISIONING_DONE
 * \enduml
 */
class WifiStateMachine {
    public:

    using internal_event_callback_t = etl::delegate<void(InternalEvent)>;
    using user_event_callback_t = etl::delegate<void(UserEvent)>;

    WifiStateMachine(
            internal_event_callback_t a_send_event_internal,
            user_event_callback_t a_send_event_user);

    using transition = etl::state_chart_traits::transition<WifiStateMachine>;
    using state = etl::state_chart_traits::state<WifiStateMachine>;

    static const transition transition_table[9];
    static const state state_table[5];

    private:
    void check_if_provisioned();
    // construct and start provisioning
    void provisioning_start();
    // destruct provisioning
    void provisioning_done();
    void start();
    void connect_with_timeout();
    void connect_to_ap();
    bool is_reconnecting();
    void on_connected();
    void cleanup_timer();
    void on_disconnected();

    static constexpr char TAG[] = "wifi_state_machine";

    internal_event_callback_t send_event_internal;
    user_event_callback_t send_event_user;

    std::unique_ptr<wifi_provisioning::WiFiProvisioning> provisioning = nullptr;

    TickType_t connect_start_tick_count = 0;
};

}
}
