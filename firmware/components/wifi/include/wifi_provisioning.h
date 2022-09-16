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

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <queue.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_softap.h>

namespace shrapnel {
namespace wifi_provisioning {

class WiFiProvisioning final {
private:

    enum event_type_t {
        EVENT_PROVISIONING_SUCCESSFUL,
        EVENT_PROVISIONING_FAILED,
    };

    struct Event {
        event_type_t type;
    };

    static constexpr char TAG[5] = "wifi";

    Queue<Event> queue;

static void provisioning_event_handler(void* arg, esp_event_base_t event_base,
                                       int32_t event_id, void* event_data)
{
    assert(arg != nullptr);
    assert(event_base == WIFI_PROV_EVENT);

    auto queue = static_cast<Queue<Event> *>(arg);

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

                Event event{.type = EVENT_PROVISIONING_FAILED};
                int rc = queue->send(&event, 100 / portTICK_PERIOD_MS);
                if(rc != true)
                {
                    ESP_LOGE(TAG, "failed to send to queue");
                }
                break;
            }
            case WIFI_PROV_CRED_SUCCESS: {
                ESP_LOGI(TAG, "Provisioning successful");

                Event event{.type = EVENT_PROVISIONING_SUCCESSFUL};
                int rc = queue->send(&event, 100 / portTICK_PERIOD_MS);
                if(rc != true)
                {
                    ESP_LOGE(TAG, "failed to send to queue");
                }
                break;
            }
            case WIFI_PROV_END:
                ESP_LOGI(TAG, "Provisioning end");
                break;
            default:
                ESP_LOGE(TAG, "Unhandled provisioning event: %d", event_id);
                break;
        }
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
   (void) arg;

   if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // TODO this is causing an error when not provisioned. Is it doing
        // anything productive?
        ESP_LOGE(TAG, "\n\n\n\n\n=======================================================================");
        ESP_LOGE(TAG, "calling connect");
        int rc = esp_wifi_connect();
        if(rc != ESP_OK)
        {
            ESP_LOGE(TAG, "wifi connect failed %d", rc);
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
    } else {
        ESP_LOGE(TAG, "Unhandled wifi event: %s %d", event_base, event_id);
    }
}

static void get_device_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    const char *ssid_prefix = "shrapnel_";
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%s%02X%02X%02X",
             ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
}

public:
WiFiProvisioning(void) : queue(1)
{
    // XXX: Make sure to deregister the handlers in the destructor
    ESP_ERROR_CHECK(esp_event_handler_register(
                WIFI_PROV_EVENT,
                ESP_EVENT_ANY_ID,
                provisioning_event_handler,
                &queue));
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
        .scheme = wifi_prov_scheme_softap,
    };

    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));
}

static bool is_provisioned(void)
{
    bool provisioned = false;
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));
    return provisioned;
}

static void start(void)
{
    ESP_LOGI(TAG, "Starting provisioning");

    char service_name[16];
    get_device_service_name(service_name, sizeof(service_name));

    wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
    const char *pop = "abcd1234";
    const char *service_key = NULL;

    ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));
}

void wait_for_provisioning(void)
{
    start();

    while(1)
    {
        Event event;
        int rc = queue.receive(&event, portMAX_DELAY);
        assert(rc == true);

        if(event.type == EVENT_PROVISIONING_SUCCESSFUL)
        {
            break;
        }
        else if(event.type == EVENT_PROVISIONING_FAILED)
        {
            wifi_prov_mgr_stop_provisioning();
            wifi_prov_mgr_wait();
            start();
        }
        else
        {
            ESP_LOGE(TAG, "unhandled event %d", event.type);
        }
    }

    ESP_LOGI(TAG, "Provisioning finished");
}


~WiFiProvisioning() {
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

};

}
}
