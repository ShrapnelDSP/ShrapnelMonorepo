include(../../../thirdparty/esp-idf-components/nanopb/nanopb-generate-cpp.cmake)

add_library(cmd_handling STATIC)
add_library(shrapnel::cmd_handling ALIAS cmd_handling)

set(NANOPB_GENERATE_CPP_APPEND_PATH ON)
nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS
                    ../../../proto/cmd_handling.proto)

target_sources(cmd_handling
               PRIVATE
               ${PROTO_SRCS}
               src/cmd_handling.cpp
               src/cmd_handling_api.cpp)

target_include_directories(cmd_handling
                           PUBLIC
                           include
                           # for generated proto headers
                           ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(cmd_handling
                      PUBLIC
                      idf::esp_common
                      idf::log
                      idf::freertos
                      shrapnel::api
                      shrapnel::audio_param
                      shrapnel::etl
                      shrapnel::nanopb
                      PRIVATE
                      shrapnel::audio_events
                      shrapnel::compiler_warning_flags)

if(DEFINED TESTING)
    add_executable(cmd_handling_test
                   test/test_cmd_handling.cpp)

    target_link_libraries(cmd_handling_test PRIVATE shrapnel::cmd_handling)

    target_link_libraries(cmd_handling_test
                          PRIVATE
                          GTest::gtest_main
                          GTest::gmock
                          shrapnel::compiler_warning_flags)
    gtest_discover_tests(cmd_handling_test)
endif()
