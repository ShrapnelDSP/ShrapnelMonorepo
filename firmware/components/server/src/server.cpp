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

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "server.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "messages.h"
#include "os/debug.h"
#include <etl/string_stream.h>

#define MAX_CLIENTS 3
#define TAG "server"
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

namespace shrapnel {

static void debug_print_sent_message(const ApiMessage &message);
static void debug_print_received_message(const ApiMessage &message);

Server::Server(QueueBase<AppMessage> *a_in_queue,
               QueueBase<AppMessage> *a_out_queue)
{
    in_queue = a_in_queue;
    out_queue = a_out_queue;
    work_semaphore = xSemaphoreCreateBinary();
    assert(work_semaphore);
    xSemaphoreGive(work_semaphore);
}

esp_err_t websocket_get_handler(httpd_req_t *req);

void Server::start()
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);

    httpd_handle_t new_server = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;
    config.ctrl_port = 8081;
    config.max_open_sockets = MAX_CLIENTS;
    config.stack_size = 5700;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    esp_err_t rc = httpd_start(&new_server, &config);
    if(rc == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");

        static const httpd_uri_t websocket = {
            .uri = "/websocket",
            .method = HTTP_GET,
            .handler = websocket_get_handler,
            .user_ctx = this,
            .is_websocket = true,
            .handle_ws_control_frames = false,
            .supported_subprotocol = nullptr,
        };
        httpd_register_uri_handler(new_server, &websocket);
        server = new_server;
    }
    else
    {
        ESP_LOGE(TAG, "Error starting server! %d %s", rc, esp_err_to_name(rc));
    }
}

void Server::stop()
{
    /* TODO need to stop tasks that use the web server first */

    // Stop the httpd server
    // XXX: this blocks until the server is stopped
    httpd_stop(server);
}

esp_err_t websocket_get_handler(httpd_req_t *req)
{
    auto self = reinterpret_cast<Server *>(req->user_ctx);
    std::array<uint8_t, 1024> buffer_data = {0};

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_BINARY,
        .payload = buffer_data.data(),
        .len = 0,
    };

    if(req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Got websocket upgrade request");
        ESP_LOGI(TAG,
                 "%s stack %d",
                 __FUNCTION__,
                 uxTaskGetStackHighWaterMark(nullptr));
        heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
        debug_dump_task_list();
        return ESP_OK;
    }

    esp_err_t rc = httpd_ws_recv_frame(req, &pkt, buffer_data.size());
    if(rc != ESP_OK)
    {
        ESP_LOGE(TAG, "websocket parse failed");
        return rc;
    }

    /* We should never see any of these packets */
    assert(pkt.type != HTTPD_WS_TYPE_CONTINUE);
    assert(pkt.type != HTTPD_WS_TYPE_TEXT);
    assert(pkt.type != HTTPD_WS_TYPE_CLOSE);
    assert(pkt.type != HTTPD_WS_TYPE_PING);
    assert(pkt.type != HTTPD_WS_TYPE_PONG);
    assert(pkt.final);
    assert(!pkt.fragmented);
    assert(pkt.len <= buffer_data.size());

    int fd = httpd_req_to_sockfd(req);

    auto buffer = std::span<uint8_t>(buffer_data.data(), pkt.len);

    ESP_LOGD(TAG, "%s len = %zd", __FUNCTION__, buffer.size());
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer.data(), buffer.size(), ESP_LOG_VERBOSE);

    auto message = api::from_bytes<ApiMessage>(buffer);
    if(message.has_value())
    {
        debug_print_received_message(*message);
        auto out = AppMessage{*message, fd};
        auto queue_rc = self->in_queue->send(&out, pdMS_TO_TICKS(100));
        if(queue_rc != queue_error::SUCCESS)
        {
            ESP_LOGE(TAG, "in_queue message dropped");
        }
    }
    else
    {
        ESP_LOGE(TAG, "failed to parse received message");
        ESP_LOG_BUFFER_HEXDUMP(
            TAG, buffer.data(), buffer.size(), ESP_LOG_ERROR);
    }

    ESP_LOGI(
        TAG, "%s stack %d", __FUNCTION__, uxTaskGetStackHighWaterMark(nullptr));
    return ESP_OK;
}

void websocket_send(void *arg)
{
    auto self = reinterpret_cast<Server *>(arg);

    AppMessage message;
    auto rc = self->out_queue->receive(&message, 0);
    if(rc != queue_error::SUCCESS)
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

    debug_print_sent_message(message.first);

    send_websocket_message(*self, message);

    xSemaphoreGive(self->work_semaphore);
}

void send_websocket_message(Server &self, const AppMessage &message)
{
    std::array<uint8_t, 1024> memory{};
    auto buffer = std::span<uint8_t>{memory};

    auto encoded = api::to_bytes(message.first, buffer);
    if(!encoded.has_value())
    {
        ESP_LOGE(TAG, "Failed to encode message");
        return;
    }

    ESP_LOGD(TAG, "%s len = %zd", __FUNCTION__, encoded->size());
    ESP_LOG_BUFFER_HEXDUMP(
        TAG, encoded->data(), encoded->size(), ESP_LOG_VERBOSE);

    httpd_ws_frame_t pkt = {
        .final = false,
        .fragmented = false,
        .type = HTTPD_WS_TYPE_BINARY,
        .payload = encoded->data(),
        .len = encoded->size(),
    };

    int client_fds[MAX_CLIENTS];
    size_t number_of_clients = ARRAY_LENGTH(client_fds);

    int rc = httpd_get_client_list(self.server, &number_of_clients, client_fds);
    if(rc != ESP_OK)
    {
        ESP_LOGE(
            TAG, "Failed to list httpd clients %d %s", rc, esp_err_to_name(rc));
        number_of_clients = 0;
    }

    ESP_LOGD(TAG, "n = %zd", number_of_clients);

    assert(number_of_clients <= MAX_CLIENTS);

    for(size_t i = 0; i < number_of_clients; i++)
    {
        int fd = client_fds[i];

        ESP_LOGD(TAG, "fd = %d", fd);

        if(HTTPD_WS_CLIENT_WEBSOCKET != httpd_ws_get_fd_info(self.server, fd))
        {
            continue;
        }

        if(message.second.has_value() && fd == *message.second)
        {
            continue;
        }

        rc = httpd_ws_send_frame_async(self.server, fd, &pkt);
        if(ESP_OK != rc)
        {
            ESP_LOGE(TAG, "failed to send to fd %d rc %d", fd, rc);
        }
    }
}

static void debug_print_sent_message(const ApiMessage &message)
{
    etl::string<128> debug;
    etl::string_stream debug_stream{debug};
    std::visit(
        [&](const auto &message) -> void
        {
            debug_stream << message;
            ESP_LOGD(TAG, "sending message: %s", debug.data());
        },
        message);
}

static void debug_print_received_message(const ApiMessage &message)
{
    etl::string<128> debug;
    etl::string_stream debug_stream{debug};
    std::visit(
        [&](const auto &message) -> void
        {
            debug_stream << message;
            ESP_LOGD(TAG, "received message: %s", debug.data());
        },
        message);
}

void Server::send_message(const AppMessage &message)
{
    ESP_LOGD(
        TAG, "%s called from task: %s", __FUNCTION__, pcTaskGetName(nullptr));

    if(queue_error::SUCCESS != out_queue->send(&message, pdMS_TO_TICKS(100)))
    {
        ESP_LOGE(TAG, "Failed to send message to websocket");
        return;
    }

    if(!xSemaphoreTake(work_semaphore, 1000 / portTICK_PERIOD_MS))
    {
        ESP_LOGE(TAG, "work semaphore timed out");
        return;
    }

    // XXX: server is accessed from outside the main thread here. It is
    // probably incorrect, and will break when the server is being torn down
    // concurrently to an audio event going out.
    esp_err_t rc = httpd_queue_work(server, websocket_send, this);
    if(ESP_OK != rc)
    {
        ESP_LOGE(TAG, "failed to queue work for server");
    }
}

} // namespace shrapnel