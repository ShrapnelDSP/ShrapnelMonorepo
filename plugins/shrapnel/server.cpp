#include "server.h"
#include "App.h"
#include <iostream>

int server_main()
{
    /* Overly simple hello world app */
    uWS::App()
        .get("/*",
             [](auto *res, auto * /*req*/)
             {
                 std::cout << "Received request" << std::endl;
                 res->end("Hello world!");
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

shrapnel::Server::Server(shrapnel::QueueBase<AppMessage> *in_queue,
                         shrapnel::QueueBase<AppMessage> *out_queue)
    : Thread("server")
{
}

void shrapnel::Server::start() { startThread(); }

void shrapnel::Server::send_message(const AppMessage &message) {}

void shrapnel::Server::run()
{
    // TODO monitor threadShouldExit and stop when required
    server_main();
}
