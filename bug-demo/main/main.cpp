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

#include <stdio.h>
#include <math.h>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#define TAG "main"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <assert.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "mdns.h"

#include "esp_http_server.h"

#define QUEUE_LEN 20
#define MAX_CLIENTS 3
#define MAX_WEBSOCKET_PAYLOAD_SIZE 128
#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

static httpd_handle_t server = NULL;

extern "C" {

static void work(void *arg);
static esp_err_t websocket_get_handler(httpd_req_t *req);
static httpd_handle_t start_webserver(void);
static void stop_webserver(httpd_handle_t server);
static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
static void start_mdns(void);

void create_work(void);

}

static esp_err_t websocket_get_handler(httpd_req_t *req)
{
    return 0;
}

static const httpd_uri_t websocket = {
    .uri       = "/websocket",
    .method    = HTTP_GET,
    .handler   = websocket_get_handler,
    .user_ctx  = NULL,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = NULL,
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = MAX_CLIENTS;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &websocket);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    for(int i = 0; i < QUEUE_LEN; i++)
    {
        //vTaskDelay(100 / portTICK_PERIOD_MS); // this delay makes it work
        create_work();
    }
}

typedef struct {
    char message[MAX_WEBSOCKET_PAYLOAD_SIZE];
    int fd;
} audio_event_message_t;

void create_work(void)
{
    esp_err_t rc = httpd_queue_work(server, work, server);
    if(ESP_OK != rc)
    {
        ESP_LOGE(TAG, "failed to queue work for server");
    }
}

static void work(void *arg)
{
    static int i = 0;
    i++;

    ESP_LOGI(TAG, "%s %d", __FUNCTION__, i);

    assert((!(i == 20)) && "working as expected");
}

static void start_mdns(void)
{
    esp_err_t err = mdns_init();
    if (err) {
        ESP_LOGE(TAG, "MDNS Init failed: %d %s", err, esp_err_to_name(err));
        return;
    }

    mdns_hostname_set("guitar-dsp");
    mdns_instance_name_set("Barabas' Guitar Processor");

    mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    mdns_service_instance_name_set("_http", "_tcp", "Barabas' Guitar Processor Web Server");
}

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Start the mdns service */
    start_mdns();

    ESP_LOGI(TAG, "setup done");
}
