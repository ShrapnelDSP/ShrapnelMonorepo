include(../../thirdparty/esp-idf-components/nanopb/nanopb-generate-cpp.cmake)

add_library(audio_events STATIC)
add_library(shrapnel::audio_events ALIAS audio_events)

set(NANOPB_GENERATE_CPP_APPEND_PATH ON)
nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS
                    ../../proto/audio_events.proto)

target_include_directories(audio_events
                           PUBLIC
                           include
                           # for generated proto headers
                           ${CMAKE_CURRENT_BINARY_DIR})
                           
target_sources(audio_events PRIVATE ${PROTO_SRCS} src/audio_events.cpp)

target_link_libraries(audio_events
                      PUBLIC
                      shrapnel::api
                      shrapnel::nanopb
                      shrapnel::etl
                      PRIVATE
                      idf::log
                      shrapnel::compiler_warning_flags)