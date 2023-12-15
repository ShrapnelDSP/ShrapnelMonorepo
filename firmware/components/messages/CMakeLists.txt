add_library(messages INTERFACE)
add_library(shrapnel::messages ALIAS messages)

target_include_directories(messages INTERFACE include)

target_link_libraries(messages
        INTERFACE
        shrapnel::audio_events
        shrapnel::cmd_handling
        shrapnel::midi_mapping
        shrapnel::presets)
