if(NOT DEFINED TESTING)
    idf_component_register(SRCS "audio_param.c"
                           INCLUDE_DIRS "include"
                           PRIV_REQUIRES "i2s")
else()
    # TODO generate the mocks with CppUMock
    add_library(audio_param_mock mocks/audio_param.c)
    target_include_directories(audio_param_mock PUBLIC include)

    target_link_libraries(audio_param_mock PUBLIC esp_common)
endif()
