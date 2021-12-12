if(NOT DEFINED TESTING)
    idf_component_register(SRCS "audio_param.c"
                           INCLUDE_DIRS "include"
                           PRIV_REQUIRES "i2s")
else()
    add_library(audio_param_mock mocks/audio_param.cpp)
    target_include_directories(audio_param_mock PUBLIC include)

    target_link_libraries(audio_param_mock PUBLIC esp_common)
endif()
