if(NOT DEFINED TESTING)
    idf_component_register( SRCS src/esp_persistence.cpp
                            INCLUDE_DIRS include
                            REQUIRES etl
                            PRIV_REQUIRES nvs_flash)
else()
endif()
