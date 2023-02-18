if(ESP_PLATFORM)
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
else()
add_library(main INTERFACE)
add_library(shrapnel::main ALIAS main)

target_include_directories(main INTERFACE include)
target_link_libraries(main INTERFACE
        shrapnel::audio_events
        shrapnel::cmd_handling
        shrapnel::etl
        shrapnel::midi_protocol
        shrapnel::midi_mapping
        shrapnel::persistence
        shrapnel::timer
        shrapnel::queue)
endif()
