if(NOT DEFINED TESTING)
    add_library(cmd_handling STATIC)
    add_library(shrapnel::cmd_handling ALIAS cmd_handling)

    target_sources(cmd_handling PRIVATE
                   src/cmd_handling.cpp
                   src/cmd_handling_json.cpp
                   src/cmd_handling_json_builder.cpp)

    target_include_directories(cmd_handling PUBLIC include)

    target_link_libraries(cmd_handling
                          PUBLIC
                          shrapnel::rapidjson
                          shrapnel::audio_param
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
            idf::esp_common
            shrapnel::etl
            idf::freertos
            idf::log
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
