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

#include "wifi_state_machine.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_softap.h"

#define CONNECT_TIMEOUT_MS 15'000

namespace {

constexpr char TAG[] = "wifi_state_machine";

void get_device_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    const char *ssid_prefix = "shrapnel_";
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name,
             max,
             "%s%02X%02X%02X",
             ssid_prefix,
             eth_mac[3],
             eth_mac[4],
             eth_mac[5]);
}

void provisioning_event_handler(void* arg, esp_event_base_t event_base,
                                       int32_t event_id, void* event_data)
{
    assert(arg != nullptr);
    assert(event_base == WIFI_PROV_EVENT);

    auto send_event = static_cast<
        shrapnel::wifi::WifiStateMachine::internal_event_callback_t *>(arg);

    if (event_base == WIFI_PROV_EVENT) {
        switch (event_id) {
        case WIFI_PROV_INIT:
            ESP_LOGI(TAG, "Provisioning initialised");
            break;
        case WIFI_PROV_START:
            ESP_LOGI(TAG, "Provisioning started");
            break;
        case WIFI_PROV_CRED_RECV: {
            wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
            ESP_LOGI(TAG, "Received Wi-Fi credentials");
            ESP_LOGI(TAG, "SSID:");
            ESP_LOG_BUFFER_HEXDUMP(
                TAG,
                wifi_sta_cfg->ssid,
                sizeof(wifi_sta_cfg->ssid),
                ESP_LOG_INFO);
            ESP_LOGI(TAG, "Password:");
            ESP_LOG_BUFFER_HEXDUMP( TAG,
                                    wifi_sta_cfg->password,
                                    sizeof(wifi_sta_cfg->password),
                                    ESP_LOG_INFO);
            break;
        }
        case WIFI_PROV_CRED_FAIL: {
            auto reason = (wifi_prov_sta_fail_reason_t *)event_data;
            ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s",
                     (*reason == WIFI_PROV_STA_AUTH_ERROR) ?
                     "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");

            (*send_event)(shrapnel::wifi::InternalEvent::PROVISIONING_FAILURE);
            break;
        }
        case WIFI_PROV_CRED_SUCCESS: {
            ESP_LOGI(TAG, "Provisioning successful");

            (*send_event)(shrapnel::wifi::InternalEvent::PROVISIONING_SUCCESS);
            break;
        }
        case WIFI_PROV_END:
            ESP_LOGI(TAG, "Provisioning end");
            (*send_event)(shrapnel::wifi::InternalEvent::PROVISIONING_END);
            break;
        default:
            ESP_LOGW(TAG, "Unhandled provisioning event: %d", event_id);
            break;
        }
    }
}

void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    (void) arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "WIFI_EVENT_SCAN_DONE");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGE(TAG, "WIFI_EVENT_STA_DISCONNECTED");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_START) {
        ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STOP) {
        ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        ESP_LOGE(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
    } else {
        ESP_LOGW(TAG, "Unhandled wifi event: %s %d", event_base, event_id);
    }
}

bool is_provisioned(void)
{
    bool provisioned = false;

    wifi_config_t wifi_cfg;
    ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg));

    if (wifi_cfg.sta.ssid[0] != '\0') {
        provisioned = true;
    }
    return provisioned;
}


} // namespace

namespace shrapnel {
namespace wifi {

WifiStateMachine::WifiStateMachine(
            internal_event_callback_t a_send_event_internal,
            user_event_callback_t a_send_event_user) :
        send_event_internal(a_send_event_internal),
        send_event_user(a_send_event_user)
    {};

constexpr WifiStateMachine::transition WifiStateMachine::transition_table[]{
    // clang-format off
    transition(State::INIT,         InternalEvent::IS_PROVISIONED,     State::STARTING),
    transition(State::INIT,         InternalEvent::IS_NOT_PROVISIONED, State::PROVISIONING, &WifiStateMachine::provisioning_init),

    transition(State::STARTING,     InternalEvent::STARTED,            State::CONNECTING),

    // Espressif uses the disconnect event to indicate failed connection. Retry
    // when we receive this event, but only if it has been less than
    // CONNECT_TIMEOUT_MS since we have started connecting. Otherwise, return
    // to the provisioning state.
    transition(State::CONNECTING,             InternalEvent::DISCONNECT,           State::CONNECTING,             &WifiStateMachine::connect_to_ap, &WifiStateMachine::is_reconnecting),
    transition(State::CONNECTING,             InternalEvent::DISCONNECT,           State::PROVISIONING,           &WifiStateMachine::provisioning_init),
    transition(State::CONNECTING,             InternalEvent::CONNECT_SUCCESS,      State::CONNECTED),

    transition(State::CONNECTED,              InternalEvent::DISCONNECT,           State::CONNECTING),
    transition(State::CONNECTED,              InternalEvent::RESET_PROVISIONING,   State::PROVISIONING,           &WifiStateMachine::provisioning_init),

    transition(State::PROVISIONING,           InternalEvent::PROVISIONING_SUCCESS, State::PROVISIONING_FINISHING),
    transition(State::PROVISIONING,           InternalEvent::PROVISIONING_FAILURE, State::PROVISIONING_STOPPING),

    transition(State::PROVISIONING_FINISHING, InternalEvent::PROVISIONING_END,     State::CONNECTED,              &WifiStateMachine::provisioning_deinit),

    transition(State::PROVISIONING_STOPPING,  InternalEvent::PROVISIONING_END,     State::PROVISIONING),
    // clang-format on
};

constexpr WifiStateMachine::state WifiStateMachine::state_table[]{
    // clang-format off
    state(State::INIT,         &WifiStateMachine::check_if_provisioned),
    state(State::STARTING,     &WifiStateMachine::start),
    state(State::CONNECTING,   &WifiStateMachine::connect_with_timeout),
    state(State::CONNECTED,    &WifiStateMachine::on_connected,       &WifiStateMachine::on_disconnected),
    state(State::PROVISIONING, &WifiStateMachine::provisioning_start, &WifiStateMachine::provisioning_stop),
    state(State::PROVISIONING_FINISHING),
    state(State::PROVISIONING_STOPPING),
    // clang-format on
};

void WifiStateMachine::check_if_provisioned()
{
    bool is_provisioned = ::is_provisioned();
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

void WifiStateMachine::provisioning_init()
{
    // XXX: Make sure to deregister the handlers in the destructor
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_PROV_EVENT,
        ESP_EVENT_ANY_ID,
        provisioning_event_handler,
        &send_event_internal));
    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        wifi_event_handler,
        nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        wifi_event_handler,
        nullptr));

    wifi_prov_mgr_config_t config = {
        .scheme{wifi_prov_scheme_softap},
        .scheme_event_handler{.event_cb{nullptr}, .user_data{nullptr}},
        .app_event_handler{.event_cb{nullptr}, .user_data{nullptr}},
    };

    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));
}


void WifiStateMachine::provisioning_deinit()
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(
        WIFI_PROV_EVENT,
        ESP_EVENT_ANY_ID,
        provisioning_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        wifi_event_handler));

    wifi_prov_mgr_deinit();
}

void WifiStateMachine::provisioning_start()
{
    ESP_LOGI(TAG, "provisioning start");

    char service_name[16];
    get_device_service_name(service_name, sizeof(service_name));

    wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
    constexpr char proof_of_possession[]{"abcd1234"};
    const char *service_key = nullptr;
    ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(
        security, &proof_of_possession, service_name, service_key));
}

void WifiStateMachine::provisioning_stop()
{
    ESP_LOGI(TAG, "provisioning stop");
    wifi_prov_mgr_stop_provisioning();
}

}
}
