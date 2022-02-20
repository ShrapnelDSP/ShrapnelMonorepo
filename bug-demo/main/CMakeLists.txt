idf_component_register( SRCS main.cpp
                        REQUIRES
                            driver
                            esp_http_server
                            protocol_examples_common
                            nvs_flash
                            mdns)
