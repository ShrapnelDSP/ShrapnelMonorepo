idf_component_register( SRCS main.c
                        REQUIRES driver nvs_flash protocol_examples_common mdns i2s esp_http_websocket_server
                        EMBED_FILES index.html )
