#include "etl/state_chart.h"

namespace shrapnel {
namespace wifi {

class WifiStateMachine : public etl::state_chart<WifiStateMachine> {
    enum Event {
        IS_PROVISIONED,
        IS_NOT_PROVISIONED,
        CONNECT_SUCCESS,
        CONNECT_TIMEOUT,
        DISCONNECT,
        RESET_PROVISIONING,
        PROVISIONING_DONE,
    };

    enum State {
        INIT,
        CONNECTING,
        CONNECTED,
        PROVISIONING,
    };

    void check_if_provisioned();
    // construct and start provisioning
    void provisioning_start();
    // destruct provisioning
    void provisioning_done();
    // connect to AP
    void connect_to_ap();
    // notify app that connection was successful
    void on_connected();
    // notify app that we have disconnected
    void on_disconnected();

    static const etl::array<WifiStateMachine::transition, 7> transitionTable;
    static const etl::array<WifiStateMachine::state, 4> state_table;

    static constexpr char TAG[] = "wifi_state_machine";
};

}
}
