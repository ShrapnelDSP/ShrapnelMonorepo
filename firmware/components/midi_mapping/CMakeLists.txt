if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/midi_mapping.cpp
                                src/midi_mapping_api.cpp
                                src/midi_mapping_json_parser.cpp
                                src/midi_mapping_json_builder.cpp
                           INCLUDE_DIRS include
                           REQUIRES midi_protocol rapidjson)

    target_link_libraries(${COMPONENT_LIB}
                          PUBLIC
                          shrapnel::audio_param
                          shrapnel::etl
                          PRIVATE
                          shrapnel::compiler_warning_flags)
else()
    add_library(midi_mapping STATIC)
    add_library(shrapnel::midi_mapping ALIAS midi_mapping)

    target_sources(midi_mapping PRIVATE
                   src/midi_mapping.cpp
                   src/midi_mapping_api.cpp
                   src/midi_mapping_json_parser.cpp
                   src/midi_mapping_json_builder.cpp)

    target_include_directories(midi_mapping PUBLIC include)

    target_link_libraries(midi_mapping
            PUBLIC
            shrapnel::audio_param
            shrapnel::midi_protocol
            shrapnel::rapidjson
            PRIVATE
            shrapnel::compiler_warning_flags)

    add_executable(midi_mapping_test
                   test/test_midi_mapping.cpp
                   test/test_json_parser.cpp
                   test/test_json_builder.cpp)

    target_link_libraries(midi_mapping_test
            PRIVATE
            shrapnel::etl
            shrapnel::log
            shrapnel::midi_mapping
            shrapnel::compiler_warning_flags
            GTest::gtest_main
            GTest::gmock)

    gtest_discover_tests(midi_mapping_test)
endif()
