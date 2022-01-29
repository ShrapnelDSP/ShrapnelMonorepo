if(NOT DEFINED TESTING)
    idf_component_register(SRCS "src/audio_param.cpp"
                           INCLUDE_DIRS "include"
                           PRIV_REQUIRES i2s gate)
else()
    add_library(audio_param_mock INTERFACE)
    target_include_directories(audio_param_mock INTERFACE include)

    target_link_libraries(audio_param_mock INTERFACE esp_common)

    add_executable(audio_param_test
                   src/audio_param.cpp
                   test/test_audio_param.cpp
                   )

    target_include_directories(audio_param_test PRIVATE include)

    target_link_libraries(audio_param_test PRIVATE etl)
    target_link_libraries(audio_param_test PRIVATE gtest_main)
    target_link_libraries(audio_param_test PRIVATE gmock)
    gtest_discover_tests(audio_param_test)

endif()
