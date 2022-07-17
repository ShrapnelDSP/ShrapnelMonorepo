if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/midi_uart.cpp
                           INCLUDE_DIRS include
                           PRIV_REQUIRES driver compiler_warning_flags)
else()
    message(FATAL_ERROR TODO)
endif()
