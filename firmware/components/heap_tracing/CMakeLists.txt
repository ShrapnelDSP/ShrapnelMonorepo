if(NOT DEFINED TESTING)
    idf_component_register(INCLUDE_DIRS include
                           REQUIRES heap
                           PRIV_REQUIRES compiler_warning_flags)
else()
    message(FATAL_ERROR "Heap tracing is not testable")
endif()
