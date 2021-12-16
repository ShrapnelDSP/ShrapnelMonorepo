if(NOT DEFINED TESTING)
    idf_component_register(SRCS "src/cmd_handling.c"
                           INCLUDE_DIRS "include"
                           PRIV_REQUIRES json audio_param task)
else()
    add_executable(cmd_handling_test
                   src/cmd_handling.c
                   test/test_cmd_handling.cpp
                   )

    target_include_directories(cmd_handling_test PRIVATE include)

    target_link_libraries(cmd_handling_test PRIVATE audio_param_mock)
    target_link_libraries(cmd_handling_test PRIVATE json)
    target_link_libraries(cmd_handling_test PRIVATE esp_common)
    target_link_libraries(cmd_handling_test PRIVATE freertos)
    target_link_libraries(cmd_handling_test PRIVATE log)
    target_link_libraries(cmd_handling_test PRIVATE task_mock)

    target_link_libraries(cmd_handling_test PRIVATE gtest_main)
    gtest_discover_tests(cmd_handling_test)

#   set(testcases QueueFail InvalidMessage ValidMessage)
#   foreach(testcase ${testcases})
#       add_test(NAME cmd_handling.${testcase}
#           COMMAND cmd_handling_test -t "cmd_handling.${testcase}")
#       set_property(TEST cmd_handling.${testcase}
#           PROPERTY LABELS cmd_handling)
#   endforeach()
endif()
