#pragma once

#include "esp_http_server.h"
#include "queue.h"
#include "messages.h"

namespace shrapnel {

void init_webserver(
    QueueBase<AppMessage> *in_queue,
    QueueBase<AppMessage> *out_queue
    );

httpd_handle_t start_webserver();
void stop_webserver(httpd_handle_t server);

void server_send_message(httpd_handle_t server, const AppMessage &message);

}
