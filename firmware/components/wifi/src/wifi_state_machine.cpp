#include "wifi_state_machine.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "wifi_provisioning.h"

namespace shrapnel {
namespace wifi {

constexpr WifiStateMachine::transition WifiStateMachine::transition_table[]{
    WifiStateMachine::transition(INIT, IS_PROVISIONED, STARTING),
    WifiStateMachine::transition(INIT, IS_NOT_PROVISIONED, PROVISIONING),

    WifiStateMachine::transition(STARTING, STARTED, CONNECTING),

    WifiStateMachine::transition(CONNECTING, CONNECT_SUCCESS, CONNECTED),
    WifiStateMachine::transition(CONNECTING, CONNECT_TIMEOUT, PROVISIONING),

    WifiStateMachine::transition(CONNECTED, DISCONNECT, CONNECTING),
    WifiStateMachine::transition(CONNECTED, RESET_PROVISIONING, PROVISIONING),

    WifiStateMachine::transition(PROVISIONING, PROVISIONING_DONE, CONNECTED),
};

constexpr WifiStateMachine::state WifiStateMachine::state_table[]{
    WifiStateMachine::state(INIT, &WifiStateMachine::check_if_provisioned, nullptr),
    WifiStateMachine::state(STARTING, &WifiStateMachine::start, nullptr),
    WifiStateMachine::state(CONNECTING, &WifiStateMachine::connect_to_ap, nullptr),
    WifiStateMachine::state(CONNECTED, &WifiStateMachine::on_connected, nullptr),
    WifiStateMachine::state(
            PROVISIONING,
            &WifiStateMachine::provisioning_start,
            &WifiStateMachine::provisioning_done),
};

void WifiStateMachine::check_if_provisioned()
{
    bool is_provisioned = wifi_provisioning::WiFiProvisioning::is_provisioned();
#if SHRAPNEL_RESET_WIFI_CREDENTIALS
    ESP_LOGW(TAG, "Reseting wifi provisioning");
    is_provisioned = false;
#endif
    send_event(is_provisioned ? IS_PROVISIONED : IS_NOT_PROVISIONED);
}

void WifiStateMachine::start()
{
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void WifiStateMachine::connect_to_ap()
{
    int rc = esp_wifi_connect();
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "wifi connect failed %d", rc);
    }
}

void WifiStateMachine::on_connected()
{
    // TODO let the application configure what happens here. We will probably
    // want to send an event to some other part of the app, that will cause the
    // server to start.
    ESP_LOGI(TAG, "wifi connected");
}

void WifiStateMachine::provisioning_start()
{
    ESP_LOGI(TAG, "provisioning start");

    assert(!provisioning.get());
    provisioning = std::make_unique<wifi_provisioning::WiFiProvisioning>();
    // TODO merge these state machines into a single state machine?
    //      This blocks the calling thread until provisioning is complete.
    //
    //      There is a simple state machine inside this function, it should be
    //      easy to merge to here, and make it not block.
    provisioning->wait_for_provisioning();
}

void WifiStateMachine::provisioning_done()
{
    ESP_LOGI(TAG, "provisioning done");

    assert(provisioning.get());
    provisioning = nullptr;
}

}
}
