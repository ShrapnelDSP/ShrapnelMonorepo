if(ESP_PLATFORM)
idf_component_register(INCLUDE_DIRS "include"
                       REQUIRES driver
                       PRIV_REQUIRES compiler_warning_flags)
endif()
