#include "wifi_state_machine.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "wifi_provisioning.h"

#define CONNECT_TIMEOUT_MS 15'000

namespace shrapnel {
namespace wifi {

WifiStateMachine::WifiStateMachine(
            internal_event_callback_t a_send_event_internal,
            user_event_callback_t a_send_event_user) :
        send_event_internal(a_send_event_internal),
        send_event_user(a_send_event_user)
    {};

constexpr WifiStateMachine::transition WifiStateMachine::transition_table[]{
    transition(State::INIT,         InternalEvent::IS_PROVISIONED,     State::STARTING),
    transition(State::INIT,         InternalEvent::IS_NOT_PROVISIONED, State::PROVISIONING),

    transition(State::STARTING,     InternalEvent::STARTED,            State::CONNECTING),

    // Espressif uses the disconnect event to indicate failed connection. Retry
    // when we receive this event, but only if it has been less than
    // CONNECT_TIMEOUT_MS since we have started connecting. Otherwise, return
    // to the provisioning state.
    transition(State::CONNECTING,   InternalEvent::DISCONNECT,         State::CONNECTING, &WifiStateMachine::connect_to_ap, &WifiStateMachine::is_reconnecting),
    transition(State::CONNECTING,   InternalEvent::DISCONNECT,         State::PROVISIONING),
    transition(State::CONNECTING,   InternalEvent::CONNECT_SUCCESS,    State::CONNECTED),

    transition(State::CONNECTED,    InternalEvent::DISCONNECT,         State::CONNECTING),
    transition(State::CONNECTED,    InternalEvent::RESET_PROVISIONING, State::PROVISIONING),

    transition(State::PROVISIONING, InternalEvent::PROVISIONING_DONE,  State::CONNECTED),
};

constexpr WifiStateMachine::state WifiStateMachine::state_table[]{
    state(State::INIT, &WifiStateMachine::check_if_provisioned, nullptr),
    state(State::STARTING, &WifiStateMachine::start, nullptr),
    state(State::CONNECTING, &WifiStateMachine::connect_with_timeout, nullptr),
    state(State::CONNECTED, &WifiStateMachine::on_connected, &WifiStateMachine::on_disconnected),
    state( State::PROVISIONING, &WifiStateMachine::provisioning_start, &WifiStateMachine::provisioning_done),
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

void WifiStateMachine::connect_with_timeout()
{
    connect_start_tick_count = xTaskGetTickCount();
    connect_to_ap();
}

void WifiStateMachine::connect_to_ap()
{
    ESP_LOGI(TAG, "Trying to connect to configured AP");
    int rc = esp_wifi_connect();
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "wifi connect failed %d", rc);
    }
}

bool WifiStateMachine::is_reconnecting()
{
    bool is_reconnecting = (xTaskGetTickCount() - connect_start_tick_count) < CONNECT_TIMEOUT_MS;
    if(is_reconnecting)
    {
        ESP_LOGI(TAG, "Retrying connection");
    }
    else
    {
        ESP_LOGI(TAG, "Connection timed out, giving up");
    }
    return is_reconnecting;
}

void WifiStateMachine::on_connected()
{
    ESP_LOGI(TAG, "wifi connected");
    send_event_user(UserEvent::CONNECTED);
}

void WifiStateMachine::on_disconnected()
{
    ESP_LOGI(TAG, "wifi disconnected");
    send_event_user(UserEvent::DISCONNECTED);
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
