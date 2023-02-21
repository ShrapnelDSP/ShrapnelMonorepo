if(NOT DEFINED TESTING)
    idf_component_register( SRCS src/esp_persistence.cpp
                            INCLUDE_DIRS include
                            PRIV_REQUIRES nvs_flash)

    target_link_libraries(${COMPONENT_LIB}
                          PUBLIC
                          shrapnel::etl
                          PRIVATE
                          shrapnel::compiler_warning_flags)
else()
    add_library(persistence INTERFACE)
    add_library(shrapnel::persistence ALIAS persistence)

    target_include_directories(persistence INTERFACE include)

    target_link_libraries(persistence INTERFACE shrapnel::etl)
endif()
