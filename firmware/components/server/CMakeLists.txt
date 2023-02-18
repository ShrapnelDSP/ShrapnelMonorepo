if(ESP_PLATFORM)
    idf_component_register(SRCS src/server.cpp
                           INCLUDE_DIRS include
                           REQUIRES
                           esp_http_server
                           queue
                           messages
                           PRIV_REQUIRES compiler_warning_flags)
endif()
