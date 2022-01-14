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

#include "audio_param.h"
#include "audio_events.h"
#include "cmd_handling_task.h"
#include "esp_http_server.h"
#include "hardware.h"
#include "i2s.h"
#include "pcm3060.h"
#include "profiling.h"

#define QUEUE_LEN 10
#define MAX_CLIENTS 3
#define MAX_WEBSOCKET_PAYLOAD_SIZE 128
#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

static shrapnel::Queue<shrapnel::CommandHandling::Message> *in_queue;
static shrapnel::AudioParameters *audio_params;
static shrapnel::CommandHandlingTask *cmd_handling_task;

static QueueHandle_t out_queue;
static httpd_handle_t server = NULL;

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
static void websocket_send(void *arg);
static void start_mdns(void);
static void i2c_setup(void);

}

static esp_err_t websocket_get_handler(httpd_req_t *req)
{
    shrapnel::CommandHandling::Message message = { 0 };

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = reinterpret_cast<uint8_t*>(message.json),
        .len = 0,
    };

    if(req->method == HTTP_GET)
    {
        ESP_LOGE(TAG, "before handshake");
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
    ESP_LOG_BUFFER_HEXDUMP(TAG, message.json, sizeof(message.json), ESP_LOG_DEBUG);

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
    const size_t html_size = (html_end - html_start);

    httpd_resp_send(req, (const char *)html_start, html_size);
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
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

static const httpd_uri_t ui = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = ui_get_handler,
    .user_ctx  = NULL,
    .is_websocket = false,
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
        httpd_register_uri_handler(server, &ui);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
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
}

void audio_event_send_callback(const char *message)
{
    if(errQUEUE_FULL ==
            xQueueSendToBack(out_queue,
                             message,
                             100 / portTICK_PERIOD_MS))
    {
        ESP_LOGE(TAG, "Failed to send message to websocket");
    }

    esp_err_t rc = httpd_queue_work(server, websocket_send, server);
    if(ESP_OK != rc)
    {
        ESP_LOGE(TAG, "failed to queue work for server");
        xQueueReset(out_queue);
    }
}

static void websocket_send(void *arg)
{
    httpd_handle_t server = arg;

    char buffer[MAX_WEBSOCKET_PAYLOAD_SIZE + 1] = {0};

    int rc = xQueueReceive(out_queue, buffer, 0);
    if(!rc)
    {
        ESP_LOGE(TAG, "%s failed to receive from queue", __FUNCTION__);
        return;
    }

    ESP_LOGD(TAG, "%s len = %zd", __FUNCTION__, strlen(buffer));
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, sizeof(buffer), ESP_LOG_DEBUG);

    assert(strlen(buffer) < sizeof(buffer));

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = reinterpret_cast<uint8_t *>(buffer),
        .len = strlen(buffer),
    };

    int client_fds[MAX_CLIENTS];
    size_t number_of_clients = ARRAY_LENGTH(client_fds);

    httpd_get_client_list((httpd_handle_t) arg, &number_of_clients, client_fds);

    ESP_LOGD(TAG, "n = %zd", number_of_clients);

    assert(number_of_clients <= MAX_CLIENTS);

    for(int i = 0; i < number_of_clients; i++)
    {
        int fd = client_fds[i];

        ESP_LOGD(TAG, "fd = %d", fd);

        if(HTTPD_WS_CLIENT_WEBSOCKET != httpd_ws_get_fd_info(server, fd))
        {
            continue;
        }

        rc = httpd_ws_send_frame_async(server, fd, &pkt);
        if(ESP_OK != rc)
        {
            ESP_LOGE(TAG, "failed to send to fd %d rc %d", fd, rc);
        }
    }
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

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    /* Start the server for the first time */
    server = start_webserver();

    in_queue = new shrapnel::Queue<shrapnel::CommandHandling::Message>(QUEUE_LEN);
    assert(in_queue);

    out_queue = xQueueCreate(QUEUE_LEN, MAX_WEBSOCKET_PAYLOAD_SIZE);
    assert(out_queue);

    audio_params = new shrapnel::AudioParameters();

    cmd_handling_task = new shrapnel::CommandHandlingTask(5, in_queue, audio_params);
    cmd_handling_task->cmd.register_parameter({
            .name = "tight",
            .id = PARAM_TIGHT,
            });
    cmd_handling_task->cmd.register_parameter({
            .name = "hmGain",
            .id = PARAM_HM2_GAIN,
            });
    cmd_handling_task->cmd.register_parameter({
            .name = "ampGain",
            .id = PARAM_AMP_GAIN,
            });
    cmd_handling_task->cmd.register_parameter({
            .name = "bass",
            .id = PARAM_BASS,
            });
    cmd_handling_task->cmd.register_parameter({
            .name = "middle",
            .id = PARAM_MIDDLE,
            });
    cmd_handling_task->cmd.register_parameter({
            .name = "treble",
            .id = PARAM_TREBLE,
            });
    cmd_handling_task->cmd.register_parameter({
            .name = "volume",
            .id = PARAM_VOLUME,
            });
    cmd_handling_task->cmd.register_parameter({
            .name = "gateThreshold",
            .id = PARAM_GATE_THRESHOLD,
            });

    ESP_ERROR_CHECK(audio_event_init(out_queue));

    i2c_setup();

    profiling_mutex = xSemaphoreCreateMutex();
    i2s_setup(PROFILING_GPIO);

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
    ret = xTaskCreate(i2s_profiling_task, "i2s profiling", 2000, NULL, configMAX_PRIORITIES - 2, NULL);
    if(ret != pdPASS)
    {
        ESP_LOGE(TAG, "Profiling task create failed %d", ret);
    }
#endif

    ESP_LOGI(TAG, "setup done");
}
