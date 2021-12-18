if(NOT DEFINED TESTING)
    idf_component_register(SRCS "src/queue.c"
                           INCLUDE_DIRS "include"
                           )
else()
add_library(queue
    src/queue.cpp
    )

target_include_directories(queue PUBLIC include)

# TODO define the mock class here
#      It will be the same for all tests, so no point duplicating it
add_library(queue_mock INTERFACE)
target_include_directories(queue_mock INTERFACE include)
endif()
