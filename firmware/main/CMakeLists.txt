if(ESP_PLATFORM)
    idf_component_register(SRCS main.cpp
                           INCLUDE_DIRS include
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
                          shrapnel::etl
                          shrapnel::hardware
                          shrapnel::heap_tracing
                          shrapnel::i2s
                          shrapnel::messages
                          shrapnel::midi_mapping
                          shrapnel::midi_protocol
                          shrapnel::os
                          shrapnel::pcm3060
                          shrapnel::persistence
                          shrapnel::presets
                          shrapnel::presets_storage
                          shrapnel::server
                          shrapnel::wifi
                          shrapnel::compiler_warning_flags)
else()
    add_library(main INTERFACE)
    add_library(shrapnel::main ALIAS main)

    target_include_directories(main INTERFACE include)
    target_link_libraries(main INTERFACE
                          shrapnel::audio_events
                          shrapnel::cmd_handling
                          shrapnel::etl
                          shrapnel::messages
                          shrapnel::midi_mapping
                          shrapnel::midi_protocol
                          shrapnel::os
                          shrapnel::persistence
                          shrapnel::presets)
endif()
