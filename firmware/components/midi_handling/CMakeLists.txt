add_library(shrapnel_midi_handling INTERFACE)
add_library(shrapnel::midi_handling ALIAS shrapnel_midi_handling)

target_include_directories(shrapnel_midi_handling INTERFACE include)

target_link_libraries(shrapnel_midi_handling INTERFACE
                      shrapnel::etl
                      shrapnel::messages
                      shrapnel::presets
)

if(DEFINED TESTING)
    add_executable(midi_handling_test
                   test/test_midi_handling.cpp)

    target_link_libraries(midi_handling_test
                          PRIVATE
                          GTest::gmock
                          GTest::gtest_main
                          shrapnel::audio_param
                          shrapnel::compiler_warning_flags
                          shrapnel::midi_handling)

    gtest_discover_tests(midi_handling_test)
endif()
