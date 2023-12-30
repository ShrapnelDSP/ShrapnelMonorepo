if(ESP_PLATFORM)
    # FIXME: move the main_thread to a separate component so the build system can
    # be used more effectively to keep it free from hardware dependencies
    # It should link to no idf:: targets, or any espressif specific targets from
    # shrapnel::
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
                          shrapnel::esp_persistence
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
