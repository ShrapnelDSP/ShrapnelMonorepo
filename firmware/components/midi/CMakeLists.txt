if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/midi_uart.cpp src/midi.cpp
                           INCLUDE_DIRS include
                           REQUIRES etl
                           PRIV_REQUIRES driver compiler_warning_flags)
else()
    add_executable(midi_test
                   src/midi.cpp
                   test/midi_util.cpp
                   test/test_midi_decoder.cpp
                   test/test_midi_message.cpp
                   )

    target_include_directories(midi_test PRIVATE include)

    target_link_libraries(midi_test PRIVATE etl)
    target_link_libraries(midi_test PRIVATE log)

    target_link_libraries(midi_test PRIVATE gtest_main)
    target_link_libraries(midi_test PRIVATE gmock)
    target_link_libraries(midi_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(midi_test)
endif()
