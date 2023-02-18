if(ESP_PLATFORM)
    idf_component_register(INCLUDE_DIRS include
                           REQUIRES
                           audio_events
                           cmd_handling
                           midi_mapping
                           PRIV_REQUIRES compiler_warning_flags)
    return()
endif()

add_library(messages INTERFACE)
add_library(shrapnel::messages ALIAS messages)

target_include_directories(messages INTERFACE include)

target_link_libraries(messages
        INTERFACE
        shrapnel::audio_events
        shrapnel::cmd_handling
        shrapnel::midi_mapping)