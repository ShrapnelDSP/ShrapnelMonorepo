if(ESP_PLATFORM)
idf_component_register(SRCS "pcm3060.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES driver i2c compiler_warning_flags)
endif()
