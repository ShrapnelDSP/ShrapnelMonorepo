if(ESP_PLATFORM)
idf_component_register(SRCS "esp32/abstract_dsp_esp32.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES "esp-dsp")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
else()

    # TODO
    #add_library(abstract_dsp STATIC
    #            juce/abstract_dsp_juce.cpp)
    #
    #target_include_directories(abstract_dsp PUBLIC include)
    #
    #target_link_libraries(abstract_dsp PRIVATE juce_modules)

    add_executable(delayline_test
                   universal/delayline.cpp
                   test/test_delayline.cpp
                   )

    target_include_directories(delayline_test PUBLIC universal)

    target_link_libraries(delayline_test PRIVATE gtest_main)
    target_link_libraries(delayline_test PRIVATE gmock)
    gtest_discover_tests(delayline_test)

endif()
