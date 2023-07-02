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

#include "esp_http_server.h"
#include "messages.h"
#include "os/queue.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace shrapnel {

class Server
{
public:
    Server(QueueBase<AppMessage> *in_queue, QueueBase<AppMessage> *out_queue);

    void start();
    void stop();

    void send_message(const AppMessage &message);

private:
    httpd_handle_t server = nullptr;

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
    friend esp_err_t websocket_get_handler(httpd_req_t *req);
    friend void websocket_send(void *arg);
};

} // namespace shrapnel
