if(NOT DEFINED TESTING)
    idf_component_register( SRCS src/esp_persistence.cpp
                            INCLUDE_DIRS include
                            REQUIRES etl
                            PRIV_REQUIRES nvs_flash)
else()
    add_library(persistence INTERFACE)
    add_library(shrapnel::persistence ALIAS persistence)

    target_include_directories(persistence INTERFACE include)

    target_link_libraries(persistence INTERFACE shrapnel::etl)
endif()
