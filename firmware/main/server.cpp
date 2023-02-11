#include "server.h"
#include "cmd_handling_json.h"
#include "cmd_handling_json_builder.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "messages.h"
#include "midi_mapping_json_parser.h"
#include "midi_mapping_json_builder.h"
#include "rapidjson/writer.h"
#include <etl/string_stream.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <rapidjson/document.h>

#define MAX_CLIENTS 3
#define TAG "server"
#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

namespace shrapnel {

namespace {
    QueueBase<AppMessage> *in_queue;
    QueueBase<AppMessage> *out_queue;

    /*
     * TODO espressif's http server drops some calls to the work function when
     * there are many calls queued at once. Waiting for the previous execution to
     * finish seems to help, but is probably not a real solution. There will be
     * some internal functions writing to the control socket which could still
     * reproduce the bug.
     */
    SemaphoreHandle_t work_semaphore;
}

void init_webserver(
    QueueBase<AppMessage> *a_in_queue,
    QueueBase<AppMessage> *a_out_queue
) {
    in_queue = a_in_queue;
    out_queue = a_out_queue;
    work_semaphore = xSemaphoreCreateBinary();
    assert(work_semaphore);
    xSemaphoreGive(work_semaphore);

}

static esp_err_t websocket_get_handler(httpd_req_t *req);

static const httpd_uri_t websocket = {
    .uri = "/websocket",
    .method = HTTP_GET,
    .handler = websocket_get_handler,
    .user_ctx = nullptr,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = nullptr,
};

httpd_handle_t start_webserver()
{
    httpd_handle_t server = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;
    config.ctrl_port = 8081;
    config.max_open_sockets = MAX_CLIENTS;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if(httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &websocket);
        return server;
    }

    ESP_LOGE(TAG, "Error starting server!");
    return nullptr;
}

void stop_webserver(httpd_handle_t server)
{
    /* TODO need to stop tasks that use the web server first */

    // Stop the httpd server
    // XXX: this blocks until the server is stopped
    httpd_stop(server);
}

static esp_err_t websocket_get_handler(httpd_req_t *req)
{
    /* Largest incoming message is a MidiMap::create::response which has a maximum size about 200 bytes */
    char json[256] = {0};

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = reinterpret_cast<uint8_t *>(json),
        .len = 0,
    };

    if(req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Got websocket upgrade request");
        heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
        return ESP_OK;
    }

    esp_err_t rc = httpd_ws_recv_frame(req, &pkt, sizeof(json));
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
    assert(pkt.len <= sizeof(json));

    ESP_LOGD(TAG, "%s len = %zd", __FUNCTION__, pkt.len);
    ESP_LOG_BUFFER_HEXDUMP(TAG, json, sizeof(json), ESP_LOG_VERBOSE);

    int fd = httpd_req_to_sockfd(req);

    rapidjson::Document document;
    document.Parse(json);
    if(document.HasParseError())
    {
        ESP_LOGE(TAG, "Failed to parse incoming JSON");
        return ESP_FAIL;
    }

    {
        auto message =
            parameters::from_json<parameters::ApiMessage>(document.GetObject());
        if(message.has_value())
        {
            auto out = AppMessage{*message, fd};
            int queue_rc = in_queue->send(&out, pdMS_TO_TICKS(100));
            if(queue_rc != pdPASS)
            {
                ESP_LOGE(TAG, "in_queue message dropped");
            }
            goto out;
        }
    }

    {
        auto message =
            midi::from_json<midi::MappingApiMessage>(document.GetObject());
        if(message.has_value())
        {
            auto out = AppMessage{*message, fd};
            int queue_rc = in_queue->send(&out, pdMS_TO_TICKS(100));
            if(queue_rc != pdPASS)
            {
                ESP_LOGE(TAG, "in_queue message dropped");
            }

            etl::string<256> buffer;
            etl::string_stream stream{buffer};
            stream << *message;
            ESP_LOGI(TAG, "decoded %s", buffer.data());

            goto out;
        }
    }

out:
    ESP_LOGI(
        TAG, "%s stack %d", __FUNCTION__, uxTaskGetStackHighWaterMark(nullptr));
    return ESP_OK;
}

static void websocket_send(void *arg)
{
    httpd_handle_t server = arg;

    AppMessage message;
    int rc = out_queue->receive(&message, 0);
    if(!rc)
    {
        ESP_LOGE(TAG, "%s failed to receive from queue", __FUNCTION__);
        return;
    }

    if(!message.second.has_value())
    {
        ESP_LOGD(TAG, "%s source fd is null", __FUNCTION__);
    }
    else
    {
        ESP_LOGD(TAG, "%s source fd = %d", __FUNCTION__, *message.second);
    }

    rapidjson::Document document;

    auto json = std::visit([&](const auto &message) -> rapidjson::Value
                           { return to_json(document, message); },
                           message.first);

    rapidjson::GenericStringBuffer<rapidjson::UTF8<>,
                                   rapidjson::MemoryPoolAllocator<>>
        buffer;
    rapidjson::Writer writer{buffer};
    json.Swap(document);
    document.Accept(writer);

    const char *payload = buffer.GetString();
    std::size_t payload_len = strlen(payload);

    ESP_LOGD(TAG, "%s len = %zd", __FUNCTION__, payload_len);
    ESP_LOG_BUFFER_HEXDUMP(TAG, payload, payload_len, ESP_LOG_VERBOSE);

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_TEXT,
        // XXX: casting away const, but esp-idf does not modify the buffer
        .payload = reinterpret_cast<uint8_t *>(const_cast<char *>(payload)),
        .len = payload_len,
    };

    int client_fds[MAX_CLIENTS];
    size_t number_of_clients = ARRAY_LENGTH(client_fds);

    httpd_get_client_list((httpd_handle_t)arg, &number_of_clients, client_fds);

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

        if(message.second.has_value() && fd == *message.second)
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

void server_send_message(httpd_handle_t server, const AppMessage &message)
{
    ESP_LOGD(TAG, "%s %s", __FUNCTION__, pcTaskGetName(nullptr));

    if(errQUEUE_FULL ==
       shrapnel::out_queue->send(&message, pdMS_TO_TICKS(100)))
    {
        ESP_LOGE(TAG, "Failed to send message to websocket");
        return;
    }

    if(!xSemaphoreTake(shrapnel::work_semaphore, 1000 / portTICK_PERIOD_MS))
    {
        ESP_LOGE(TAG, "work semaphore timed out");
        return;
    }

    // XXX: server is accessed from outside the main thread here. It is
    // probably incorrect, and will break when the server is being torn down
    // concurrently to an audio event going out.
    esp_err_t rc = httpd_queue_work(
        server,
        shrapnel::websocket_send,
        server);
    if(ESP_OK != rc)
    {
        ESP_LOGE(TAG, "failed to queue work for server");
    }
}

}