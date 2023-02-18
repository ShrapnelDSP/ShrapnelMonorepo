if(NOT DEFINED TESTING)
    idf_component_register(SRCS "src/audio_param.cpp"
                           INCLUDE_DIRS "include"
                           PRIV_REQUIRES compiler_warning_flags
                           REQUIRES etl)
else()
    add_library(audio_param STATIC)
    add_library(shrapnel::audio_param ALIAS audio_param)

    target_sources(audio_param PRIVATE src/audio_param.cpp)

    target_include_directories(audio_param PUBLIC include)

    target_link_libraries(audio_param PUBLIC
            shrapnel::etl
            shrapnel::log)

    add_executable(audio_param_test
                   src/audio_param.cpp
                   test/test_audio_param.cpp)

    target_link_libraries(audio_param_test PRIVATE
            shrapnel::audio_param
            GTest::gtest_main
            GTest::gmock
            shrapnel::compiler_warning_flags)
    gtest_discover_tests(audio_param_test)

endif()
