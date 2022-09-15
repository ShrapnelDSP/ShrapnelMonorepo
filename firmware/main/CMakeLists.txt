idf_component_register( SRCS main.cpp
                        REQUIRES
                            audio_events
                            cmd_handling
                            driver
                            esp_http_server
                            hardware
                            i2s
                            mdns
                            midi
                            nvs_flash
                            pcm3060
                            queue
                            wifi
                        PRIV_REQUIRES compiler_warning_flags
                        EMBED_FILES index.html
                        LDFRAGMENTS esp-dsp.lf
                        )
