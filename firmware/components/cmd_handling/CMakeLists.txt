if(NOT DEFINED TESTING)
    idf_component_register(SRCS "src/cmd_handling.c"
                           INCLUDE_DIRS "include"
                           PRIV_REQUIRES json audio_param task)
else()
    add_executable(cmd_handling_test
                   src/cmd_handling.c
                   test/test_cmd_handling.cpp
                   $ENV{IDF_PATH}/components/json/cJSON/cJSON.c
                   )

    # TODO modern cmake says these should be modules we can include
    #      We could make some interface modules that have the relevant include
    #      paths in the test folder, and then depend on those
    target_include_directories(cmd_handling_test PRIVATE $ENV{IDF_PATH}/components/json/cJSON)
    target_include_directories(cmd_handling_test PRIVATE $ENV{IDF_PATH}/components/esp_common/include)
    target_include_directories(cmd_handling_test PRIVATE include)
    target_include_directories(cmd_handling_test PRIVATE ../audio_param/include)
    target_include_directories(cmd_handling_test PRIVATE ../task/include)

    add_test(NAME cmd_handling_test
             COMMAND cmd_handling_test
             )
endif()
