#include "wifi_state_machine.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "wifi_provisioning.h"

namespace shrapnel {
namespace wifi {

constexpr WifiStateMachine::transition WifiStateMachine::transition_table[]{
    WifiStateMachine::transition(State::INIT,         InternalEvent::IS_PROVISIONED,     State::STARTING),
    WifiStateMachine::transition(State::INIT,         InternalEvent::IS_NOT_PROVISIONED, State::PROVISIONING),

    WifiStateMachine::transition(State::STARTING,     InternalEvent::STARTED,            State::CONNECTING),

    WifiStateMachine::transition(State::CONNECTING,   InternalEvent::CONNECT_SUCCESS,    State::CONNECTED),
    WifiStateMachine::transition(State::CONNECTING,   InternalEvent::CONNECT_TIMEOUT,    State::PROVISIONING),

    WifiStateMachine::transition(State::CONNECTED,    InternalEvent::DISCONNECT,         State::CONNECTING),
    WifiStateMachine::transition(State::CONNECTED,    InternalEvent::RESET_PROVISIONING, State::PROVISIONING),

    WifiStateMachine::transition(State::PROVISIONING, InternalEvent::PROVISIONING_DONE,  State::CONNECTED),
};

constexpr WifiStateMachine::state WifiStateMachine::state_table[]{
    WifiStateMachine::state(State::INIT, &WifiStateMachine::check_if_provisioned, nullptr),
    WifiStateMachine::state(State::STARTING, &WifiStateMachine::start, nullptr),
    WifiStateMachine::state(State::CONNECTING, &WifiStateMachine::connect_to_ap, nullptr),
    WifiStateMachine::state(State::CONNECTED, &WifiStateMachine::on_connected, nullptr),
    WifiStateMachine::state(
            State::PROVISIONING,
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
    send_event_internal(is_provisioned ? InternalEvent::IS_PROVISIONED : InternalEvent::IS_NOT_PROVISIONED);
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
    ESP_LOGI(TAG, "wifi connected");
    send_event_user(UserEvent::CONNECTED);
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
