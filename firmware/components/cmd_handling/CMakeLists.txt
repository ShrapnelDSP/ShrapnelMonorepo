idf_component_register(SRCS "src/cmd_handling.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES json audio_param task)

# we don't want to add all the components to requires for building the test, we
# want to use fakes instead
# This might be OK already, the esp-idf call only links the default target
# (cmd_handling) to those components
add_executable(cmd_handling_test
               src/cmd_handling.c
               test/test_cmd_handling.cpp
               #$ENV{IDF_PATH}/components/json/cJSON/cJSON.c
               )

# grab include paths from json
# does this also link to the object file?
#target_link_libraries(cmd_handling_test PUBLIC json)
target_include_directories(cmd_handling_test PRIVATE $ENV{IDF_PATH}/components/json/cJSON)
target_include_directories(cmd_handling_test PRIVATE include)
target_include_directories(cmd_handling_test PRIVATE ../audio_param/include)
target_include_directories(cmd_handling_test PRIVATE ../task/include)

add_test(NAME cmd_handling_test
         COMMAND cmd_handling_test
         )
