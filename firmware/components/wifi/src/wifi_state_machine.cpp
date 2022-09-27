#include "wifi_state_machine.h"
#include "esp_wifi.h"
#include "esp_log.h"

namespace shrapnel {
namespace wifi {

const etl::array<WifiStateMachine::transition, 7> WifiStateMachine::transitionTable{
    WifiStateMachine::transition(INIT, IS_PROVISIONED, CONNECTING),
    WifiStateMachine::transition(INIT, IS_NOT_PROVISIONED, PROVISIONING),

    WifiStateMachine::transition(CONNECTING, CONNECT_SUCCESS, CONNECTED),
    WifiStateMachine::transition(CONNECTING, CONNECT_TIMEOUT, PROVISIONING),

    WifiStateMachine::transition(CONNECTED, DISCONNECT, CONNECTING),
    // TODO when the reset even is called, we need to uninstall wifi event
    // handlers registered by main after provisioning is complete. We do not
    // want to start the websocket server while provisioning is still active.
    // Should we send all the WiFi events through this state machine so that we
    // can prevent them from reaching the app while provisioning?
    WifiStateMachine::transition(CONNECTED, RESET_PROVISIONING, PROVISIONING),

    WifiStateMachine::transition(PROVISIONING, PROVISIONING_DONE, CONNECTED),
};

const etl::array<WifiStateMachine::state, 4> WifiStateMachine::state_table{
    WifiStateMachine::state(INIT, &WifiStateMachine::check_if_provisioned, nullptr),
    WifiStateMachine::state(CONNECTING, &WifiStateMachine::connect_to_ap, nullptr),
    WifiStateMachine::state(CONNECTED, &WifiStateMachine::on_connected, nullptr),
    WifiStateMachine::state(
            PROVISIONING,
            &WifiStateMachine::provisioning_start,
            &WifiStateMachine::provisioning_done),
};

void WifiStateMachine::check_if_provisioned()
{
    // TODO look this up using esp_wifi API, or maybe refactor wifi
    // provisioning to do that
    bool provisioned = false;
    if(provisioned) {
        // TODO is this a recursive call? we might have to post this event to a
        // queue and then call process event from the main task
        process_event(IS_PROVISIONED);
    } else {
        process_event(IS_NOT_PROVISIONED);
    }
}

void WifiStateMachine::connect_to_ap()
{
    // XXX This requires that there is an event handler installed that
    //     connects to the AP in response to the start event. This is done in
    //     the main file.
    //     Should there be two state here? One waiting for the wifi start
    //     event, and the other waiting for the got IP event.
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void WifiStateMachine::on_connected()
{
    // TODO let the application configure what happens here. We will probably
    // want to send an event to some other part of the app, that will cause the
    // server to start.
    ESP_LOGI(TAG, "wifi connected");
}

}
}
