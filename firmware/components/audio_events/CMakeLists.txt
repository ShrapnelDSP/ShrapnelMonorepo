if(NOT DEFINED TESTING)
idf_component_register(SRCS "src/audio_events.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES esp_http_server compiler_warning_flags)
else()
    add_library(audio_events_mock INTERFACE)
    target_include_directories(audio_events_mock INTERFACE include)

    target_link_libraries(audio_events_mock INTERFACE esp_common)
    target_link_libraries(audio_events_mock INTERFACE freertos)
endif()
