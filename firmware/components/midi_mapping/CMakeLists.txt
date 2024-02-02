include(../../../thirdparty/esp-idf-components/nanopb/nanopb-generate-cpp.cmake)

add_library(midi_mapping STATIC)
add_library(shrapnel::midi_mapping ALIAS midi_mapping)

set(NANOPB_GENERATE_CPP_APPEND_PATH ON)
nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS
                    ../../../proto/midi_mapping.proto)

target_sources(midi_mapping
               PRIVATE
               ${PROTO_SRCS}
               src/midi_mapping.cpp
               src/midi_mapping_api.cpp)

target_include_directories(midi_mapping PUBLIC
                           include
                           # for generated proto headers
                           ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(midi_mapping
                      PUBLIC
                      shrapnel::api
                      shrapnel::audio_param
                      shrapnel::etl
                      shrapnel::midi_protocol
                      shrapnel::nanopb
                      shrapnel::persistence
                      shrapnel::presets
                      PRIVATE
                      shrapnel::compiler_warning_flags)

if(DEFINED TESTING)
    add_executable(midi_mapping_test
                   test/test_midi_mapping.cpp)

    target_link_libraries(midi_mapping_test
                          PRIVATE
                          shrapnel::etl
                          idf::log
                          shrapnel::midi_mapping
                          shrapnel::compiler_warning_flags
                          GTest::gtest_main
                          GTest::gmock)

    gtest_discover_tests(midi_mapping_test)
endif()
