if(NOT DEFINED TESTING)
    idf_component_register(SRCS "src/task.c"
                           INCLUDE_DIRS "include"
                           )
else()
    # TODO generate the mocks with CppUMock
    add_library(task_mock mocks/task.c)
    target_include_directories(task_mock PUBLIC include)
endif()
