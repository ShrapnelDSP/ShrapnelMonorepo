if(ESP_PLATFORM)
idf_component_register(SRCS src/wifi_state_machine.cpp
                       INCLUDE_DIRS include
                       REQUIRES wifi_provisioning etl os nvs_flash)
endif()
