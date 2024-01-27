add_library(abstract_dsp STATIC)
add_library(shrapnel::abstract_dsp ALIAS abstract_dsp)

target_sources(abstract_dsp
               PRIVATE
               universal/delayline.cpp
               universal/delayline_c.cpp
               universal/fast_convolution.cpp
               universal/fast_fir.cpp
               universal/iir_universal.c)

target_include_directories(abstract_dsp PUBLIC include)

target_link_libraries(abstract_dsp
                      PRIVATE
                      shrapnel::compiler_warning_flags
                      PUBLIC
                      idf::esp-dsp)

target_compile_options(abstract_dsp PRIVATE -Wdouble-promotion)

if(ESP_PLATFORM)
    target_sources(abstract_dsp
                   PRIVATE
                   esp32/abstract_dsp_esp32.c
                   esp32/iir_3rd_order.S
                   esp32/iir_4th_order.S
                   esp32/profiling.c)

    target_include_directories(abstract_dsp PUBLIC esp32/include)

    return()
endif()

target_sources(abstract_dsp
               PRIVATE
               desktop/abstract_dsp_juce.cpp
               desktop/profiling.c)

target_include_directories(abstract_dsp PUBLIC desktop/include)

target_link_libraries(abstract_dsp PUBLIC shrapnel::juce_modules)

if(DEFINED TESTING)
    add_executable(delayline_test test/test_delayline.cpp)
    target_link_libraries(delayline_test PRIVATE abstract_dsp)
    target_link_libraries(delayline_test PRIVATE GTest::gtest_main)
    target_link_libraries(delayline_test PRIVATE GTest::gmock)
    target_link_libraries(delayline_test PRIVATE shrapnel::compiler_warning_flags)
    gtest_discover_tests(delayline_test)

    add_executable(iir_test test/test_iir.cpp)
    target_link_libraries(iir_test PRIVATE abstract_dsp)
    target_link_libraries(iir_test PRIVATE GTest::gtest_main)
    target_link_libraries(iir_test PRIVATE GTest::gmock)
    target_link_libraries(iir_test PRIVATE shrapnel::compiler_warning_flags)
    gtest_discover_tests(iir_test)

    add_executable(fast_convolution_test test/test_fast_convolution.cpp)
    target_link_libraries(fast_convolution_test PRIVATE abstract_dsp)
    target_link_libraries(fast_convolution_test PRIVATE GTest::gtest_main)
    target_link_libraries(fast_convolution_test PRIVATE GTest::gmock)
    target_link_libraries(fast_convolution_test PRIVATE shrapnel::compiler_warning_flags)
    gtest_discover_tests(fast_convolution_test)

    add_executable(fast_fir_test test/test_fast_fir.cpp)
    target_link_libraries(fast_fir_test PRIVATE abstract_dsp)
    target_link_libraries(fast_fir_test PRIVATE GTest::gtest_main)
    target_link_libraries(fast_fir_test PRIVATE GTest::gmock)
    target_link_libraries(fast_fir_test PRIVATE shrapnel::compiler_warning_flags)
    gtest_discover_tests(fast_fir_test)

    add_executable(fast_fir_integration_test test/test_fast_fir_integration.cpp)
    target_link_libraries(fast_fir_integration_test PRIVATE abstract_dsp)
    target_link_libraries(fast_fir_integration_test PRIVATE GTest::gtest_main)
    target_link_libraries(fast_fir_integration_test PRIVATE GTest::gmock)
    target_link_libraries(fast_fir_integration_test PRIVATE shrapnel::compiler_warning_flags)
    gtest_discover_tests(fast_fir_integration_test)

    add_executable(block_processor_test test/test_block_processor.cpp)
    target_link_libraries(block_processor_test PRIVATE abstract_dsp)
    target_link_libraries(block_processor_test PRIVATE GTest::gtest_main)
    target_link_libraries(block_processor_test PRIVATE GTest::gmock)
    target_link_libraries(block_processor_test PRIVATE shrapnel::compiler_warning_flags)
    gtest_discover_tests(block_processor_test)
endif()
