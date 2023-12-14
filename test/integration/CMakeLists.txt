add_executable(integration_test)
target_sources(integration_test PRIVATE integration_test.cpp)
target_link_libraries(integration_test PRIVATE shrapnel::main)
target_link_libraries(integration_test PRIVATE shrapnel::juce_modules)

target_link_libraries(integration_test PRIVATE
        GTest::gtest_main
        GTest::gmock
        shrapnel::compiler_warning_flags)
gtest_discover_tests(integration_test)
