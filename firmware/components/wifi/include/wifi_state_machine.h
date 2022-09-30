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
    ETL_END_ENUM_TYPE
};

struct UserEvent {
    enum enum_type {
        CONNECTED,
        DISCONNECTED,
    };

    ETL_DECLARE_ENUM_TYPE(UserEvent, int)
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
    ETL_END_ENUM_TYPE
};

class WifiStateMachine {
    public:
    using internal_event_callback_t = etl::delegate<void(InternalEvent)>;
    using user_event_callback_t = etl::delegate<void(UserEvent)>;

    WifiStateMachine(internal_event_callback_t a_send_event_internal, user_event_callback_t a_send_event_user) :
        send_event_internal(a_send_event_internal),
        send_event_user(a_send_event_user)
    {};

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
    void connect_to_ap();
    void on_connected();
    void on_disconnected();

    static constexpr char TAG[] = "wifi_state_machine";

    internal_event_callback_t send_event_internal;
    user_event_callback_t send_event_user;

    std::unique_ptr<wifi_provisioning::WiFiProvisioning> provisioning = nullptr;
};

}
}
