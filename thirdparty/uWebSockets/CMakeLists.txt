add_library(shrapnel_uwebsockets STATIC)
add_library(shrapnel::uwebsockets ALIAS shrapnel_uwebsockets)

target_include_directories(shrapnel_uwebsockets
                           PUBLIC
                           uWebSockets/src
                           uWebSockets/uSockets/src
)

target_compile_definitions(shrapnel_uwebsockets
                           PUBLIC
                           LIBUS_NO_SSL
                           UWS_NO_ZLIB
)

target_sources(shrapnel_uwebsockets
               PRIVATE
               ./uWebSockets/uSockets/src/bsd.c
               ./uWebSockets/uSockets/src/context.c
               ./uWebSockets/uSockets/src/crypto/openssl.c
               ./uWebSockets/uSockets/src/crypto/sni_tree.cpp
               ./uWebSockets/uSockets/src/eventing/asio.cpp
               ./uWebSockets/uSockets/src/eventing/epoll_kqueue.c
               ./uWebSockets/uSockets/src/eventing/gcd.c
               ./uWebSockets/uSockets/src/eventing/libuv.c
               ./uWebSockets/uSockets/src/io_uring/io_context.c
               ./uWebSockets/uSockets/src/io_uring/io_loop.c
               ./uWebSockets/uSockets/src/io_uring/io_socket.c
               ./uWebSockets/uSockets/src/loop.c
               ./uWebSockets/uSockets/src/quic.c
               ./uWebSockets/uSockets/src/socket.c
               ./uWebSockets/uSockets/src/udp.c
)
