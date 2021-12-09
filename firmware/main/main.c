#include <stdio.h>
#include <math.h>

#include "esp_log.h"
#define TAG "main"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp32/sha.h"
#include "mbedtls/base64.h"
#include "protocol_examples_common.h"
#include "mdns.h"

#include "i2s.h"
#include "profiling.h"
#include "esp_http_server.h"
#include "pcm3060.h"
#include "cmd_handling.h"
#include "audio_events.h"

#define PROFILING_GPIO GPIO_NUM_23

#define QUEUE_LEN 10
QueueHandle_t in_queue;
QueueHandle_t out_queue;

#define I2C_NUM I2C_NUM_0

/* TODO is the web server single threaded? Could there be 2 threads accessing
 * this at the same time? */
static uint8_t websocket_payload[1024];

static esp_err_t websocket_get_handler(httpd_req_t *req)
{
    httpd_ws_frame_t pkt = { .payload = websocket_payload };

    esp_err_t rc = httpd_ws_recv_frame(req, &pkt, sizeof(websocket_payload));
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

    ESP_LOG_BUFFER_HEXDUMP(TAG, websocket_payload, pkt.len, ESP_LOG_INFO);

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
    /* TODO what is a subprotocol? */
};

static const httpd_uri_t ui = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = ui_get_handler,
    .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

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

void start_mdns()
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
        .sda_io_num = GPIO_NUM_13,
        .scl_io_num = GPIO_NUM_12,
        .sda_pullup_en = true,
        .scl_pullup_en = true,
        .master.clk_speed = 100 * 1000,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM, &config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM, I2C_MODE_MASTER, 0, 0, 0));
}

void app_main(void)
{
    int ret;
    static httpd_handle_t server = NULL;

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

    in_queue  = xQueueCreate(QUEUE_LEN, sizeof(char*));
    assert(in_queue);

    out_queue = xQueueCreate(QUEUE_LEN, sizeof(char*));
    assert(out_queue);

    cmd_init(in_queue);

    ESP_ERROR_CHECK(audio_event_init(out_queue));

    i2c_setup();

    profiling_mutex = xSemaphoreCreateMutex();
    i2s_setup(PROFILING_GPIO);

    vTaskDelay(100 / portTICK_PERIOD_MS);

#if 0
    //dac must be powered up after the i2s clocks have stabilised
    pcm3060_init(I2C_NUM, 0);
    ESP_ERROR_CHECK(pcm3060_power_up());
#endif

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
