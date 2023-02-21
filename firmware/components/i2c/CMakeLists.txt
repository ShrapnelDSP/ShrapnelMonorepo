if(ESP_PLATFORM)
idf_component_register(SRCS "i2c.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES driver)

target_link_libraries(${COMPONENT_LIB} PRIVATE shrapnel::compiler_warning_flags)
endif()
