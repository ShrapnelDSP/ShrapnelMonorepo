#include "etl/state_chart.h"
#include "wifi_provisioning.h"
#include "etl/delegate.h"
#include <memory>

namespace shrapnel {
namespace wifi {

class WifiStateMachine {
    public:
    enum Event {
        IS_PROVISIONED,
        IS_NOT_PROVISIONED,
        STARTED,
        CONNECT_SUCCESS,
        CONNECT_TIMEOUT,
        DISCONNECT,
        RESET_PROVISIONING,
        PROVISIONING_DONE,
    };

    using SendEventCallback = etl::delegate<void(Event)>;

    WifiStateMachine(SendEventCallback a_send_event) : send_event(a_send_event) {};

    enum State {
        INIT,
        STARTING,
        CONNECTING,
        CONNECTED,
        PROVISIONING,
    };

    using transition = etl::state_chart_traits::transition<WifiStateMachine>;
    using state = etl::state_chart_traits::state<WifiStateMachine>;

    static const transition transition_table[8];
    static const state state_table[5];

    private:
    void check_if_provisioned();
    // construct and start provisioning
    void provisioning_start();
    // destruct provisioning
    void provisioning_done();
    void start();
    void connect_to_ap();
    // notify app that connection was successful
    void on_connected();
    // notify app that we have disconnected
    void on_disconnected();

    static constexpr char TAG[] = "wifi_state_machine";

    SendEventCallback send_event;

    std::unique_ptr<wifi_provisioning::WiFiProvisioning> provisioning = nullptr;
};

}
}
