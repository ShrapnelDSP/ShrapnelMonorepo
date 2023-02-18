if(ESP_PLATFORM)
idf_component_register(INCLUDE_DIRS "."
                       PRIV_REQUIRES compiler_warning_flags)
endif()
