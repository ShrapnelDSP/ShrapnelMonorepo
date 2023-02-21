if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/cmd_handling.cpp
                                src/cmd_handling_json.cpp
                                src/cmd_handling_json_builder.cpp
                           INCLUDE_DIRS include
                           REQUIRES audio_param rapidjson)

    target_link_libraries(${COMPONENT_LIB}
                          PUBLIC
                          shrapnel::etl
                          PRIVATE
                          shrapnel::audio_events
                          shrapnel::compiler_warning_flags)
else()
    add_library(cmd_handling STATIC)
    add_library(shrapnel::cmd_handling ALIAS cmd_handling)

    target_sources(cmd_handling PRIVATE
                   src/cmd_handling.cpp
                   src/cmd_handling_json.cpp
                   src/cmd_handling_json_builder.cpp)

    target_include_directories(cmd_handling PUBLIC include)

    target_link_libraries(cmd_handling PUBLIC shrapnel::audio_events
            shrapnel::audio_param
            shrapnel::esp_common
            shrapnel::etl
            shrapnel::freertos
            shrapnel::log
            shrapnel::os
            shrapnel::rapidjson)

    add_executable(cmd_handling_test
                   test/test_cmd_handling.cpp)

    target_link_libraries(cmd_handling_test PRIVATE shrapnel::cmd_handling)

    target_link_libraries(cmd_handling_test PRIVATE GTest::gtest_main
    GTest::gmock
    shrapnel::compiler_warning_flags)
    gtest_discover_tests(cmd_handling_test)
endif()
