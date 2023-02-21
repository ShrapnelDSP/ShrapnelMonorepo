if(ESP_PLATFORM)
    idf_component_register(INCLUDE_DIRS include
                           REQUIRES
                           midi_mapping)

    target_link_libraries(${COMPONENT_LIB} INTERFACE shrapnel::cmd_handling shrapnel::audio_events)
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
