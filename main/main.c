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
#include "esp_http_websocket_server.h"

//this magic is used during the websocket handshake
const char WEBSOCKET_KEY_MAGIC[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
#define STRLEN(a) (sizeof(a)/sizeof(char))

#define QUEUE_LEN 10
QueueHandle_t in_queue;
QueueHandle_t out_queue;

static esp_err_t websocket_get_handler(httpd_req_t *req)
{
    const size_t buf_len = 100;
    size_t len;
    char   buf[buf_len];

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (len > 1) {
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
    }

    len = httpd_req_get_hdr_value_len(req, "Upgrade") + 1;
    if (len > 1) {
        if (httpd_req_get_hdr_value_str(req, "Upgrade", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Upgrade: %s", buf);
        }
        else
        {
            ESP_LOGE(TAG, "Upgrade header error");
        }
    }
    else
    {
        ESP_LOGE(TAG, "Websocket request contained no Upgrade header");
    }

    len = httpd_req_get_hdr_value_len(req, "Connection") + 1;
    if (len > 1) {
        if (httpd_req_get_hdr_value_str(req, "Connection", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Connection: %s", buf);
        }
        else
        {
            ESP_LOGE(TAG, "Connection header error");
        }
    }
    else
    {
        ESP_LOGE(TAG, "Websocket request contained no Connection header");
    }

    len = httpd_req_get_hdr_value_len(req, "Sec-WebSocket-Key") + 1;
    if (len > 1) {
        if (httpd_req_get_hdr_value_str(req, "Sec-WebSocket-Key", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Sec-WebSocket-Key: %s", buf);
        }
        else
        {
            ESP_LOGE(TAG, "Sec-WebSocket-Key header error");
        }
    }
    else
    {
        ESP_LOGE(TAG, "Websocket request contained no Sec-WebSocket-Key header");
    }

    //concatenate the magic to the request's key
    strcat(buf, WEBSOCKET_KEY_MAGIC);

    //calculate the sha1 of this
    //sha1 is always 20 bytes long
    unsigned char sha1[20];
    esp_sha(SHA1, (unsigned char *)buf, strlen(buf), sha1);

    //base64 encode the sha1
    size_t olen;
    mbedtls_base64_encode((unsigned char *)buf, buf_len - 1, &olen, sha1, 20);
    ESP_LOGI(TAG, "Base64 encoded key length: %d", olen);

    /* Response */
    httpd_resp_set_hdr(req, "Upgrade", "websocket");
    httpd_resp_set_hdr(req, "Connection", "Upgrade");
    httpd_resp_set_hdr(req, "Sec-WebSocket-Accept", buf);
    httpd_resp_set_status(req, "101 Switching Protocols");
    httpd_resp_send(req, NULL, 0);

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
    .user_ctx  = NULL
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

    in_queue  = xQueueCreate(QUEUE_LEN, sizeof(char*));
    if(in_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create in queue");
        return NULL;
    }

    out_queue = xQueueCreate(QUEUE_LEN, sizeof(char*));
    if(out_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create out queue");
        return NULL;
    }

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config, in_queue, out_queue) == ESP_OK) {
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

static void i2s_gain_task(void *param)
{
    char *s;
    int gain_db;
    int ret;
    float gain;
    while(1)
    {
        ret = xQueueReceive(in_queue, &s, portMAX_DELAY);
        if(ret == pdTRUE)
        {
            ESP_LOGI(TAG, "received websocket message: %s", s);

            gain_db = atoi(s);
            gain = powf(10.f, gain_db / 20.f);
            ESP_LOGI(TAG, "gain is: %d dB, %01.3f", gain_db, gain);

            i2s_set_gain(gain);

            //this should be the last step, as s will be deallocated on the
            //other side of this queue
            ret = xQueueSend(out_queue, &s, 100/portTICK_PERIOD_MS);
            if(ret != pdTRUE)
            {
                ESP_LOGE(TAG, "Queue failed to send");
            }
        }
        else
        {
            ESP_LOGE(TAG, "Queue failed to receive");
        }
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

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
}

void app_main(void)
{
    int ret;
    static httpd_handle_t server = NULL;

    i2c_setup();
    i2s_setup();

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

    /* Start the mdns service */
    start_mdns();


    ret = xTaskCreate(i2s_gain_task, "i2s gain", 2000, NULL, configMAX_PRIORITIES - 1, NULL);
    if(ret != pdPASS)
    {
        ESP_LOGE(TAG, "Gain task create failed %d", ret);
    }

    ESP_LOGI(TAG, "setup done");
}
