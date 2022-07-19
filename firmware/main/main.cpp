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

#include <midi.h>
#include <stdio.h>
#include <math.h>
#include <sstream>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include <assert.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_debug_helpers.h"
#include "mdns.h"

#include "audio_param.h"
#include "audio_events.h"
#include "cmd_handling_task.h"
#include "esp_http_server.h"
#include "hardware.h"
#include "i2s.h"
#include "midi_uart.h"
#include "pcm3060.h"
#include "profiling.h"
#include "wifi_provisioning.h"

#define TAG "main"
#define QUEUE_LEN 20
#define MAX_CLIENTS 3
#define MAX_WEBSOCKET_PAYLOAD_SIZE 128
#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

namespace shrapnel {

static Queue<CommandHandling<AudioParameters>::Message> *in_queue;
static AudioParameters *audio_params;
static CommandHandlingTask<AudioParameters> *cmd_handling_task;
static EventSend event_send{};

static QueueHandle_t out_queue;

/*
 * TODO espressif's http server drops some calls to the work function when
 * there are many calls queued at once. Waiting for the previous execution to
 * finish seems to help, but is probably not a real solution. There will be
 * some internal functions writing to the control socket which could still
 * reproduce the bug.
 */
static SemaphoreHandle_t work_semaphore;

static httpd_handle_t _server = nullptr;

extern "C" {

static void websocket_send(void *arg);
static esp_err_t websocket_get_handler(httpd_req_t *req);
static esp_err_t ui_get_handler(httpd_req_t *req);
static httpd_handle_t start_webserver(void);
static void stop_webserver(httpd_handle_t server);
static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);
static void start_mdns(void);
static void i2c_setup(void);

}

static esp_err_t websocket_get_handler(httpd_req_t *req)
{
    CommandHandling<AudioParameters>::Message message{};

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = reinterpret_cast<uint8_t*>(message.json),
        .len = 0,
    };

    if(req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Got websocket upgrade request");
        return ESP_OK;
    }

    esp_err_t rc = httpd_ws_recv_frame(req, &pkt, sizeof(message.json));
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "websocket parse failed");
        return rc;
    }

    /* We should never see any of these packets */
    assert(pkt.type != HTTPD_WS_TYPE_CONTINUE);
    assert(pkt.type != HTTPD_WS_TYPE_BINARY);
    assert(pkt.type != HTTPD_WS_TYPE_CLOSE);
    assert(pkt.type != HTTPD_WS_TYPE_PING);
    assert(pkt.type != HTTPD_WS_TYPE_PONG);
    assert(pkt.final);
    assert(!pkt.fragmented);
    assert(pkt.len <= sizeof(message.json));

    ESP_LOGD(TAG, "%s len = %zd", __FUNCTION__, pkt.len);
    ESP_LOG_BUFFER_HEXDUMP(TAG, message.json, sizeof(message.json), ESP_LOG_VERBOSE);

    message.fd = httpd_req_to_sockfd(req);

    rc = in_queue->send(&message, 100 / portTICK_PERIOD_MS);
    if(rc != pdTRUE)
    {
        ESP_LOGE(TAG, "%s failed to send to queue", __FUNCTION__);
    }

    ESP_LOGI(TAG, "%s stack %d", __FUNCTION__, uxTaskGetStackHighWaterMark(NULL));

    return ESP_OK;
}

static esp_err_t ui_get_handler(httpd_req_t *req)
{
    extern const unsigned char html_start[] asm("_binary_index_html_start");
    extern const unsigned char html_end[] asm("_binary_index_html_end");
    const int html_size = (html_end - html_start);

    httpd_resp_send(req, (const char *)html_start, html_size);
    httpd_resp_send(req, nullptr, 0);

    return ESP_OK;
}

static const httpd_uri_t websocket = {
    .uri       = "/websocket",
    .method    = HTTP_GET,
    .handler   = websocket_get_handler,
    .user_ctx  = nullptr,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = nullptr,
};

static const httpd_uri_t ui = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = ui_get_handler,
    .user_ctx  = nullptr,
    .is_websocket = false,
    .handle_ws_control_frames = false,
    .supported_subprotocol = nullptr,
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;
    config.ctrl_port = 8081;
    config.max_open_sockets = MAX_CLIENTS;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &websocket);
        httpd_register_uri_handler(server, &ui);
        return server;
    }

    ESP_LOGE(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    /* TODO need to stop tasks that use the web server first */

    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    (void)event_base;
    (void)event_id;
    (void)event_data;

    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = nullptr;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    (void)event_base;
    (void)event_id;
    (void)event_data;

    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == nullptr) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

static void wifi_start_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    (void)event_data;
    (void)arg;
    assert(event_base == WIFI_EVENT);
    assert(event_id == WIFI_EVENT_STA_START);

    int rc = esp_wifi_connect();
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "wifi connect failed %d", rc);
    }
}

typedef struct {
    char message[MAX_WEBSOCKET_PAYLOAD_SIZE];
    int fd;
} audio_event_message_t;

static void websocket_send(void *arg)
{
    httpd_handle_t server = arg;

    audio_event_message_t event_message;

    int rc = xQueueReceive(out_queue, &event_message, 0);
    if(!rc)
    {
        ESP_LOGE(TAG, "%s failed to receive from queue", __FUNCTION__);
        return;
    }

    ESP_LOGD(TAG, "%s %d messages waiting", __FUNCTION__, uxQueueMessagesWaiting(out_queue));

    ESP_LOGD(TAG, "%s source fd = %d", __FUNCTION__, event_message.fd);

    ESP_LOGD(TAG, "%s len = %zd", __FUNCTION__, strlen(event_message.message));
    ESP_LOG_BUFFER_HEXDUMP(TAG, event_message.message, sizeof(event_message.message), ESP_LOG_VERBOSE);

    assert(strlen(event_message.message) < sizeof(event_message.message));

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = reinterpret_cast<uint8_t *>(event_message.message),
        .len = strlen(event_message.message),
    };

    int client_fds[MAX_CLIENTS];
    size_t number_of_clients = ARRAY_LENGTH(client_fds);

    httpd_get_client_list((httpd_handle_t) arg, &number_of_clients, client_fds);

    ESP_LOGD(TAG, "n = %zd", number_of_clients);

    assert(number_of_clients <= MAX_CLIENTS);

    for(size_t i = 0; i < number_of_clients; i++)
    {
        int fd = client_fds[i];

        ESP_LOGD(TAG, "fd = %d", fd);

        if(HTTPD_WS_CLIENT_WEBSOCKET != httpd_ws_get_fd_info(server, fd))
        {
            continue;
        }

        if(fd == event_message.fd)
        {
            continue;
        }

        rc = httpd_ws_send_frame_async(server, fd, &pkt);
        if(ESP_OK != rc)
        {
            ESP_LOGE(TAG, "failed to send to fd %d rc %d", fd, rc);
        }
    }

    xSemaphoreGive(work_semaphore);
}

static void start_mdns(void)
{
    esp_err_t err = mdns_init();
    if (err) {
        ESP_LOGE(TAG, "MDNS Init failed: %d %s", err, esp_err_to_name(err));
        return;
    }

    err = mdns_hostname_set("guitar-dsp");
    if(err) {
        ESP_LOGE(TAG, "MDNS failed to set host name %d %s", err, esp_err_to_name(err));
        return;
    }
    err = mdns_instance_name_set("Barabas' Guitar Processor");
    if(err) {
        ESP_LOGE(TAG, "MDNS failed to set instance name %d %s", err, esp_err_to_name(err));
        return;
    }

    err = mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
    if(err) {
        ESP_LOGE(TAG, "MDNS failed to add service %d %s", err, esp_err_to_name(err));
        return;
    }
    err = mdns_service_instance_name_set("_http", "_tcp", "Barabas' Guitar Processor Web Server");
    if(err) {
        ESP_LOGE(TAG, "MDNS failed to set service name %d %s", err, esp_err_to_name(err));
        return;
    }
}

static void i2c_setup(void)
{
    const i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_I2C_SDA,
        .scl_io_num = GPIO_I2C_SCL,
        .sda_pullup_en = true,
        .scl_pullup_en = true,
        .master = {
            .clk_speed = 100 * 1000,
        },
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM, &config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM, I2C_MODE_MASTER, 0, 0, 0));
}

static void failed_alloc_callback(size_t size, uint32_t caps, const char *function_name)
{
    ESP_LOGE(TAG, "allocation failed. size=%zu caps=%08xh, function=%s", size, caps, function_name);
    heap_caps_print_heap_info(caps);
    abort();
}

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(heap_caps_register_failed_alloc_callback(failed_alloc_callback));

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    work_semaphore = xSemaphoreCreateBinary();
    assert(work_semaphore);
    xSemaphoreGive(work_semaphore);

    in_queue = new Queue<CommandHandling<AudioParameters>::Message>(QUEUE_LEN);
    assert(in_queue);

    out_queue = xQueueCreate(QUEUE_LEN, sizeof(audio_event_message_t));
    assert(out_queue);

    audio_params = new AudioParameters();

    audio_params->create_and_add_parameter("ampGain", 0, 1, 0.5);
    audio_params->create_and_add_parameter("ampChannel", 0, 1, 0);
    audio_params->create_and_add_parameter("bass", 0, 1, 0.5);
    audio_params->create_and_add_parameter("middle", 0, 1, 0.5);
    audio_params->create_and_add_parameter("treble", 0, 1, 0.5);
    //contour gets unstable when set to 0
    audio_params->create_and_add_parameter("contour", 0.01, 1, 0.5);
    audio_params->create_and_add_parameter("volume", -30, 0, -15);

    audio_params->create_and_add_parameter("noiseGateThreshold", -80, 0, -60);
    audio_params->create_and_add_parameter("noiseGateHysteresis", 0, 5, 0);
    audio_params->create_and_add_parameter("noiseGateAttack", 1, 50, 10);
    audio_params->create_and_add_parameter("noiseGateHold", 1, 250, 50);
    audio_params->create_and_add_parameter("noiseGateRelease", 1, 250, 50);
    audio_params->create_and_add_parameter("noiseGateBypass", 0, 1, 0);

    audio_params->create_and_add_parameter("chorusRate", 0.1, 4, 0.95);
    audio_params->create_and_add_parameter("chorusDepth", 0, 1, 0.3);
    audio_params->create_and_add_parameter("chorusMix", 0, 1, 0.8);
    audio_params->create_and_add_parameter("chorusBypass", 0, 1, 0);

    cmd_handling_task = new CommandHandlingTask<AudioParameters>(
            5,
            in_queue,
            audio_params,
            event_send);

    ESP_ERROR_CHECK(audio_event_init());

    i2c_setup();

    profiling_mutex = xSemaphoreCreateMutex();
    i2s_setup(PROFILING_GPIO, audio_params);

    //dac must be powered up after the i2s clocks have stabilised
    vTaskDelay(100 / portTICK_PERIOD_MS);

#if HARDWARE == HW_PCB_REV1
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << GPIO_CODEC_NRESET,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t rc = gpio_set_level(GPIO_CODEC_NRESET, 0);
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "gpio_set_level failed %d, %s", rc, esp_err_to_name(rc));
    }

    rc = gpio_config(&io_conf);
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "gpio_config failed %d, %s", rc, esp_err_to_name(rc));
    }

    vTaskDelay(5 / portTICK_PERIOD_MS);

    rc = gpio_set_level(GPIO_CODEC_NRESET, 1);
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "gpio_set_level failed %d, %s", rc, esp_err_to_name(rc));
    }

    vTaskDelay(5 / portTICK_PERIOD_MS);

#endif

    pcm3060_init(I2C_NUM, 0);
    rc = pcm3060_power_up();
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "pcm3060 power up failed");
    }

    /* Start the mdns service */
    start_mdns();

#if 0
    rc = xTaskCreate(i2s_profiling_task, "i2s profiling", 2000, NULL, tskIDLE_PRIORITY + 2, NULL);
    if(rc != pdPASS)
    {
        ESP_LOGE(TAG, "Profiling task create failed %d", rc);
    }
#endif

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Start the mdns service */
    start_mdns();

    {
        wifi_provisioning::WiFiProvisioning wifi_provisioning{};

#if SHRAPNEL_RESET_WIFI_CREDENTIALS
        ESP_LOGW(TAG, "Reseting wifi provisioning");
        wifi_prov_mgr_reset_provisioning();
#endif

        if(!wifi_provisioning.is_provisioned())
        {
             wifi_provisioning.wait_for_provisioning();
             /* start the websocket server */
             connect_handler(&_server, IP_EVENT, IP_EVENT_STA_GOT_IP, nullptr);
        }
    }

    /* TODO Need to enter provisioning mode again if the WiFi credentials are
     *      not working. Probably the AP passphrase was changed since we got
     *      provisionined.
     */

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */
    ESP_ERROR_CHECK(esp_event_handler_register(
                IP_EVENT,
                IP_EVENT_STA_GOT_IP,
                connect_handler,
                &_server));
    ESP_ERROR_CHECK(esp_event_handler_register(
                WIFI_EVENT,
                WIFI_EVENT_STA_DISCONNECTED,
                disconnect_handler,
                &_server));
    ESP_ERROR_CHECK(esp_event_handler_register(
                WIFI_EVENT,
                WIFI_EVENT_STA_START,
                wifi_start_handler,
                nullptr));

    /* Start Wi-Fi station */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "setup done");
    ESP_LOGI(TAG, "stack: %d", uxTaskGetStackHighWaterMark(NULL));

    auto midi_uart = new midi::EspMidiUart(UART_NUM_MIDI, GPIO_NUM_MIDI);
    auto midi_decoder = new midi::Decoder(
            [] (midi::Message message) {
                etl::string<32> buffer;
                etl::string_stream stream{buffer};
                stream << message;
                ESP_LOGI(TAG, "%s", buffer.data());
            }
            );

    while(1)
    {
        uint8_t byte = midi_uart->get_byte();
        ESP_LOGI(TAG, "midi got byte 0x%02x", byte);

        midi_decoder->decode(byte);
    }

}

}

extern "C" void audio_event_send_callback(const char *message, int fd)
{
    shrapnel::audio_event_message_t event_message = {
        .message = {0},
        .fd = fd,
    };

    snprintf(event_message.message, sizeof(event_message.message), "%s", message);

    ESP_LOGD(TAG, "%s %s", __FUNCTION__, event_message.message);
    ESP_LOGD(TAG, "%s %s", __FUNCTION__, pcTaskGetName(NULL));

    if(errQUEUE_FULL ==
            xQueueSendToBack(shrapnel::out_queue,
                             &event_message,
                             100 / portTICK_PERIOD_MS))
    {
        ESP_LOGE(TAG, "Failed to send message to websocket");
        return;
    }

    if(!xSemaphoreTake(shrapnel::work_semaphore, 1000 / portTICK_PERIOD_MS))
    {
        ESP_LOGE(TAG, "work semaphore timed out");
        return;
    }

    esp_err_t rc = httpd_queue_work(
            shrapnel::_server,
            shrapnel::websocket_send,
            shrapnel::_server);
    if(ESP_OK != rc)
    {
        ESP_LOGE(TAG, "failed to queue work for server");
        xQueueReset(shrapnel::out_queue);
    }
}
