if(NOT DEFINED TESTING)
    idf_component_register(INCLUDE_DIRS include
                           REQUIRES heap
                           PRIV_REQUIRES compiler_warning_flags)
endif()
