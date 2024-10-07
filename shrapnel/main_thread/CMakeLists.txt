add_library(shrapnel_main_thread INTERFACE)
add_library(shrapnel::main_thread ALIAS shrapnel_main_thread)

target_include_directories(shrapnel_main_thread INTERFACE include)

target_link_libraries(shrapnel_main_thread INTERFACE
                      shrapnel::midi_mapping
                      shrapnel::audio_param
                      shrapnel::cmd_handling
                      shrapnel::messages
                      shrapnel::midi_handling
                      shrapnel::midi_protocol
                      shrapnel::os
                      shrapnel::persistence
                      shrapnel::presets
)