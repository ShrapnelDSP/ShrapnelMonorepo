if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/cmd_handling.cpp
                                src/cmd_handling_task.cpp
                           INCLUDE_DIRS include
                           REQUIRES audio_param queue task etl json
                           PRIV_REQUIRES audio_events)
else()
    add_executable(cmd_handling_test
                   src/cmd_handling.cpp
                   test/test_cmd_handling.cpp
                   )

    target_include_directories(cmd_handling_test PRIVATE include)

    target_link_libraries(cmd_handling_test PRIVATE audio_events_mock)
    target_link_libraries(cmd_handling_test PRIVATE audio_param_mock)
    target_link_libraries(cmd_handling_test PRIVATE esp_common)
    target_link_libraries(cmd_handling_test PRIVATE etl)
    target_link_libraries(cmd_handling_test PRIVATE freertos)
    target_link_libraries(cmd_handling_test PRIVATE json)
    target_link_libraries(cmd_handling_test PRIVATE log)
    target_link_libraries(cmd_handling_test PRIVATE queue_mock)
    target_link_libraries(cmd_handling_test PRIVATE task_mock)

    target_link_libraries(cmd_handling_test PRIVATE gtest_main)
    target_link_libraries(cmd_handling_test PRIVATE gmock)
    gtest_discover_tests(cmd_handling_test)
endif()
