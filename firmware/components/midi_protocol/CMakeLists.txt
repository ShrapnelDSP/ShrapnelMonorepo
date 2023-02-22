if(NOT DEFINED TESTING)
    add_library(midi_protocol STATIC)
    add_library(shrapnel::midi_protocol ALIAS midi_protocol)

    target_sources(midi_protocol PRIVATE src/midi_uart.cpp src/midi.cpp)
    target_include_directories(midi_protocol PUBLIC include)

    target_link_libraries(midi_protocol
                          PUBLIC
                          idf::driver
                          shrapnel::etl
                          PRIVATE
                          shrapnel::compiler_warning_flags)
else()
    add_library(midi_protocol STATIC)
    add_library(shrapnel::midi_protocol ALIAS midi_protocol)

    target_sources(midi_protocol PRIVATE src/midi.cpp)

    target_include_directories(midi_protocol PUBLIC include)

    target_link_libraries(midi_protocol PUBLIC
            idf::driver
            shrapnel::etl
            idf::log)

    add_executable(midi_test
                   src/midi.cpp
                   test/midi_util.cpp
                   test/test_midi_decoder.cpp
                   test/test_midi_message.cpp
                   )

    target_include_directories(midi_test PRIVATE include)

    target_link_libraries(midi_test PRIVATE shrapnel::etl)
    target_link_libraries(midi_test PRIVATE log)

    target_link_libraries(midi_test PRIVATE gtest_main)
    target_link_libraries(midi_test PRIVATE gmock)
    target_link_libraries(midi_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(midi_test)
endif()
