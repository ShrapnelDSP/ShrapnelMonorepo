if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/timer.cpp
                           INCLUDE_DIRS include
                           REQUIRES etl
                           PRIV_REQUIRES compiler_warning_flags)
else()
    add_library(timer INTERFACE)
    add_library(shrapnel::timer ALIAS timer)

    target_include_directories(timer INTERFACE include)
    target_link_libraries(timer INTERFACE shrapnel::etl)
endif()
