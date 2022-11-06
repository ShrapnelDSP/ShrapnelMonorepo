if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/midi_uart.cpp src/midi.cpp
                           INCLUDE_DIRS include
                           REQUIRES etl audio_param midi
                           PRIV_REQUIRES compiler_warning_flags)
else()
    add_executable(midi_mapping_test
                   src/midi_mapping.cpp
                   test/test_midi_mapping.cpp
                   )

    target_include_directories(midi_mapping_test PRIVATE include)

    target_link_libraries(midi_mapping_test PRIVATE etl)
    target_link_libraries(midi_mapping_test PRIVATE log)
    target_link_libraries(midi_mapping_test PRIVATE audio_param_mock)
    target_link_libraries(midi_mapping_test PRIVATE midi_mock)

    target_link_libraries(midi_mapping_test PRIVATE gtest_main)
    target_link_libraries(midi_mapping_test PRIVATE gmock)
    target_link_libraries(midi_mapping_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(midi_mapping_test)
endif()
