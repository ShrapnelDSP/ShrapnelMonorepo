if(ESP_PLATFORM)
    idf_component_register(SRCS
                             esp32/abstract_dsp_esp32.c
                             esp32/profiling.c
                             universal/delayline.cpp
                             universal/delayline_c.cpp
                             universal/fast_convolution.cpp
                             universal/fast_fir.cpp
                             universal/iir_universal.c
                           INCLUDE_DIRS
                             include
                             esp32/include
                           PRIV_INCLUDE_DIRS universal
                           REQUIRES esp-dsp
                           PRIV_REQUIRES compiler_warning_flags)

    target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
    return()
endif()

add_library(abstract_dsp STATIC
            desktop/abstract_dsp_juce.cpp
            desktop/profiling.c
            universal/delayline.cpp
            universal/delayline_c.cpp)

target_include_directories(abstract_dsp PUBLIC include)
target_include_directories(abstract_dsp PUBLIC desktop/include)
target_include_directories(abstract_dsp PRIVATE universal)

target_link_libraries(abstract_dsp PUBLIC shrapnel::juce_modules)
target_link_libraries(abstract_dsp PUBLIC esp-dsp)
target_link_libraries(abstract_dsp PRIVATE compiler_warning_flags)

if(DEFINED TESTING)
    add_executable(delayline_test test/test_delayline.cpp)
    target_link_libraries(delayline_test PRIVATE abstract_dsp)
    target_link_libraries(delayline_test PRIVATE gtest_main)
    target_link_libraries(delayline_test PRIVATE gmock)
    target_link_libraries(delayline_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(delayline_test)

    add_executable(iir_test test/test_iir.cpp)
    target_link_libraries(iir_test PRIVATE abstract_dsp)
    target_link_libraries(iir_test PRIVATE gtest_main)
    target_link_libraries(iir_test PRIVATE gmock)
    target_link_libraries(iir_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(iir_test)

    add_executable(fast_convolution_test test/test_fast_convolution.cpp)
    target_link_libraries(fast_convolution_test PRIVATE abstract_dsp)
    target_link_libraries(fast_convolution_test PRIVATE gtest_main)
    target_link_libraries(fast_convolution_test PRIVATE gmock)
    target_link_libraries(fast_convolution_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(fast_convolution_test)

    add_executable(fast_fir_test test/test_fast_fir.cpp)
    target_link_libraries(fast_fir_test PRIVATE abstract_dsp)
    target_link_libraries(fast_fir_test PRIVATE gtest_main)
    target_link_libraries(fast_fir_test PRIVATE gmock)
    target_link_libraries(fast_fir_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(fast_fir_test)

    add_executable(fast_fir_integration_test test/test_fast_fir_integration.cpp)
    target_link_libraries(fast_fir_integration_test PRIVATE abstract_dsp)
    target_link_libraries(fast_fir_integration_test PRIVATE gtest_main)
    target_link_libraries(fast_fir_integration_test PRIVATE gmock)
    target_link_libraries(fast_fir_integration_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(fast_fir_integration_test)

    add_executable(block_processor_test test/test_block_processor.cpp)
    target_link_libraries(block_processor_test PRIVATE abstract_dsp)
    target_link_libraries(block_processor_test PRIVATE gtest_main)
    target_link_libraries(block_processor_test PRIVATE gmock)
    target_link_libraries(block_processor_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(block_processor_test)
endif()
