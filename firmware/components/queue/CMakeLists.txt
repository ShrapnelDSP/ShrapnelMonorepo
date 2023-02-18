if(NOT DEFINED TESTING)
    idf_component_register(INCLUDE_DIRS include
                           REQUIRES etl
                           PRIV_REQUIRES compiler_warning_flags)
else()
# TODO define the mock class here
#      It will be the same for all tests, so no point duplicating it
add_library(queue_mock INTERFACE)
add_library(shrapnel::queue ALIAS queue_mock)

target_include_directories(queue_mock INTERFACE include)

target_link_libraries(queue_mock INTERFACE freertos)
endif()
