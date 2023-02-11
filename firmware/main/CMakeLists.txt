idf_component_register( SRCS main.cpp server.cpp
                        INCLUDE_DIRS include
                        REQUIRES
                            audio_events
                            cmd_handling
                            driver
                            esp_http_server
                            etl
                            hardware
                            heap_tracing
                            i2s
                            mdns
                            midi_protocol
                            midi_mapping
                            nvs_flash
                            persistence
                            pcm3060
                            timer
                            queue
                            wifi
                            abstract_dsp
                        PRIV_REQUIRES compiler_warning_flags
                        LDFRAGMENTS esp-dsp.lf
                        )
