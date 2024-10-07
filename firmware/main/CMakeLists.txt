idf_component_register(SRCS main.cpp
                       REQUIRES
                       driver
                       esp_http_server
                       mdns
                       nvs_flash
                       LDFRAGMENTS esp-dsp.lf
)

target_link_libraries(${COMPONENT_LIB}
                      PRIVATE
                      shrapnel::abstract_dsp
                      shrapnel::audio_events
                      shrapnel::cmd_handling
                      shrapnel::console
                      shrapnel::esp_midi_uart
                      shrapnel::esp_persistence
                      shrapnel::etl
                      shrapnel::hardware
                      shrapnel::heap_tracing
                      shrapnel::i2s
                      shrapnel::main_thread
                      shrapnel::messages
                      shrapnel::midi_handling
                      shrapnel::midi_mapping
                      shrapnel::midi_protocol
                      shrapnel::os
                      shrapnel::pcm3060
                      shrapnel::persistence
                      shrapnel::presets
                      shrapnel::server
                      shrapnel::wifi
                      shrapnel::compiler_warning_flags)
