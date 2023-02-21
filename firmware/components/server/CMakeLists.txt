if(ESP_PLATFORM)
    idf_component_register(SRCS src/server.cpp
                           INCLUDE_DIRS include
                           REQUIRES
                           esp_http_server
                           messages
                           os)

    target_link_libraries(${COMPONENT_LIB} PRIVATE shrapnel::compiler_warning_flags)
endif()
