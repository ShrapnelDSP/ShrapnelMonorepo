if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/timer.cpp
                           INCLUDE_DIRS include
                           REQUIRES etl
                           PRIV_REQUIRES compiler_warning_flags)
else()
endif()
