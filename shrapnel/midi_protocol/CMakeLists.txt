    add_library(midi_protocol STATIC)
    add_library(shrapnel::midi_protocol ALIAS midi_protocol)

    target_sources(midi_protocol PRIVATE src/midi.cpp)
    target_include_directories(midi_protocol PUBLIC include)

    target_link_libraries(midi_protocol
                          PUBLIC
                          idf::log
                          shrapnel::etl
                          PRIVATE
                          shrapnel::compiler_warning_flags)

    if(ESP_PLATFORM)
        target_sources(midi_protocol PRIVATE src/midi_uart.cpp)
    endif()

    if(DEFINED TESTING)
        add_executable(midi_test
                       test/midi_util.cpp
                       test/test_midi_decoder.cpp
                       test/test_midi_message.cpp)

        target_include_directories(midi_test PRIVATE include)

        target_link_libraries(midi_test
                              PRIVATE
                              midi_protocol
                              GTest::gtest_main
                              GTest::gmock
                              shrapnel::compiler_warning_flags)
        gtest_discover_tests(midi_test)
    endif()
