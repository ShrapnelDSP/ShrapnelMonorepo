include(../../../thirdparty/esp-idf-components/nanopb/nanopb-generate-cpp.cmake)

add_library(messages STATIC)
add_library(shrapnel::messages ALIAS messages)

set(NANOPB_GENERATE_CPP_APPEND_PATH ON)
nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS
                    ../../../proto/audio_events.proto
                    ../../../proto/cmd_handling.proto
                    ../../../proto/shrapnel.proto
                    ../../../proto/presets.proto
                    ../../../proto/midi_mapping.proto
                    ../../../proto/uuid.proto)

target_include_directories(messages PUBLIC
                           include
                           # for generated proto headers
                           ${CMAKE_CURRENT_BINARY_DIR})
                           
target_sources(messages PRIVATE ${PROTO_SRCS} src/messages.cpp)

target_link_libraries(messages
                      PUBLIC
                      shrapnel::api
                      shrapnel::audio_events
                      shrapnel::cmd_handling
                      shrapnel::midi_mapping
                      shrapnel::presets)
