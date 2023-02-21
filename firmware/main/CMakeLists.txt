if(ESP_PLATFORM)
    idf_component_register(SRCS main.cpp
                           INCLUDE_DIRS include
                           REQUIRES
                           audio_events
                           cmd_handling
                           driver
                           esp_http_server
                           hardware
                           heap_tracing
                           i2s
                           mdns
                           messages
                           midi_protocol
                           midi_mapping
                           nvs_flash
                           persistence
                           pcm3060
                           server
                           os
                           wifi
                           abstract_dsp
                           LDFRAGMENTS esp-dsp.lf
                           )

    target_link_libraries(${COMPONENT_LIB}
                          PUBLIC
                          shrapnel::etl
                          PRIVATE
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
                          shrapnel::persistence)
endif()
