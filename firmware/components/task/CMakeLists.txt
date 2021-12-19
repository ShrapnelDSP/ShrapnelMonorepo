if(NOT DEFINED TESTING)
    idf_component_register(SRCS "src/task.cpp"
                           INCLUDE_DIRS "include"
                           )
else()
    # TODO define the mock class here
    #      It will be the same for all tests, so no point duplicating it
    add_library(task_mock INTERFACE)
    target_include_directories(task_mock INTERFACE include)
endif()
