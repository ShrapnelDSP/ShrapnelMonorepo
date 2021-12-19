idf_component_register( SRCS main.cpp
                        REQUIRES
                            audio_events
                            cmd_handling
                            driver
                            esp_http_server
                            i2s
                            mdns
                            nvs_flash
                            pcm3060
                            protocol_examples_common
                        EMBED_FILES index.html )
