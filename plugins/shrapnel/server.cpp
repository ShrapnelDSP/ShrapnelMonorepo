#include "server.h"
#include "App.h"
#include <iostream>

#define TAG "server"

namespace shrapnel {

static void debug_print_sent_message(const ApiMessage &message);
static void debug_print_received_message(const ApiMessage &message);

struct UserData
{
};

int server_main()
{
    uWS::App()
        .ws<UserData>(
            "/websocket",
            {
                .compression = uWS::CompressOptions::DISABLED,
                .maxPayloadLength = 100 * 1024 * 1024,
                .idleTimeout = 16,
                .maxBackpressure = 100 * 1024 * 1024,
                .closeOnBackpressureLimit = false,
                .resetIdleTimeoutOnSend = false,
                .sendPingsAutomatically = true,
                .upgrade =
                    [](auto *res, auto *req, auto *webSocketContext)
                {
                    /* Default handler copied from library */
                    std::cout << "upgrade" << std::endl;

                    std::string_view secWebSocketKey =
                        req->getHeader("sec-websocket-key");

                    std::string_view secWebSocketProtocol =
                        req->getHeader("sec-websocket-protocol");
                    std::string_view secWebSocketExtensions =
                        req->getHeader("sec-websocket-extensions");

                    /* Safari 15 hack */
                    if(uWS::hasBrokenCompression(req->getHeader("user-agent")))
                    {
                        secWebSocketExtensions = "";
                    }

                    res->template upgrade<UserData>({},
                                                    secWebSocketKey,
                                                    secWebSocketProtocol,
                                                    secWebSocketExtensions,
                                                    webSocketContext);
                },

                .open =
                    [](auto * /*ws*/)
                {
                    /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */
                    std::cout << "open" << std::endl;
                },
                .message =
                    [](uWS::WebSocket<false, true, UserData> *ws,
                       std::string_view message,
                       uWS::OpCode opCode)
                {
                    std::cout << "message" << std::endl;

                    // decode
                    {
                        /* We should never see any of these packets */
                        assert(opCode != uWS::OpCode::CONTINUATION);
                        assert(opCode != uWS::OpCode::TEXT);
                        assert(opCode != uWS::OpCode::CLOSE);
                        assert(opCode != uWS::OpCode::PING);
                        assert(opCode != uWS::OpCode::PONG);

                        auto fd = static_cast<LIBUS_SOCKET_DESCRIPTOR>(
                            reinterpret_cast<uintptr_t>(ws->getNativeHandle()));

                        ESP_LOGD(
                            TAG, "%s len = %zd", __FUNCTION__, message.size());
#if 0
// TODO implement hexdump
                        ESP_LOG_BUFFER_HEXDUMP(TAG,
                                               message.data(),
                                               message.size(),
                                               ESP_LOG_VERBOSE);
#endif

                        auto decoded = api::from_bytes<ApiMessage>(
                            {reinterpret_cast<const uint8_t *>(message.data()),
                             message.size()});
                        if(decoded.has_value())
                        {
                            debug_print_received_message(*decoded);
                            auto out = AppMessage{*decoded, fd};

#if 0
                            auto queue_rc =
                                self->in_queue->send(&out, pdMS_TO_TICKS(100));
                            if(queue_rc != queue_error::SUCCESS)
                            {
                                ESP_LOGE(TAG, "in_queue message dropped");
                            }
#endif
                        }
                        else
                        {
                            ESP_LOGE(TAG, "failed to parse received message");
#if 0
// TODO implement hexdump
                            ESP_LOG_BUFFER_HEXDUMP(TAG,
                                                   message.data(),
                                                   message.size(),
                                                   ESP_LOG_ERROR);
#endif
                        }
                    }

                    // print
                },
                .dropped =
                    [](auto * /*ws*/,
                       std::string_view /*message*/,
                       uWS::OpCode /*opCode*/)
                {
                    /* A message was dropped due to set maxBackpressure and closeOnBackpressureLimit limit */
                    std::cout << "dropped" << std::endl;
                },
                .drain =
                    [](auto * /*ws*/)
                {
                    /* Check ws->getBufferedAmount() here */
                    std::cout << "drain" << std::endl;
                },
                .ping =
                    [](auto * /*ws*/, std::string_view)
                {
                    /* Not implemented yet */
                    std::cout << "ping" << std::endl;
                },
                .pong =
                    [](auto * /*ws*/, std::string_view)
                {
                    /* Not implemented yet */
                    std::cout << "pong" << std::endl;
                },
                .close =
                    [](auto * /*ws*/,
                       int /*code*/,
                       std::string_view /*message*/)
                {
                    /* You may access ws->getUserData() here */
                    std::cout << "close" << std::endl;
                },
            })
        .listen(3000,
                [](auto *listen_socket)
                {
                    if(listen_socket)
                    {
                        std::cout << "Listening on port " << 3000 << std::endl;
                    }
                })
        .run();

    std::cout << "Failed to listen on port 3000" << std::endl;
}

Server::Server(shrapnel::QueueBase<AppMessage> *in_queue,
               shrapnel::QueueBase<AppMessage> *out_queue)
    : Thread("server")
{
}

void Server::start() { startThread(); }

void Server::send_message(const AppMessage &message) {}

void Server::run()
{
    // TODO monitor threadShouldExit and stop when required
    server_main();
}

static void debug_print_sent_message(const ApiMessage &message)
{
    etl::string<128> debug;
    etl::string_stream debug_stream{debug};
    debug_stream << message;
    ESP_LOGD(TAG, "sending message: %s", debug.data());
}

static void debug_print_received_message(const ApiMessage &message)
{
    etl::string<128> debug;
    etl::string_stream debug_stream{debug};
    debug_stream << message;
    ESP_LOGD(TAG, "received message: %s", debug.data());
}

} // namespace shrapnel
