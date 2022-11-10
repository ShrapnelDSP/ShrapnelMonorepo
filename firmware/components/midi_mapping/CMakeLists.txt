if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/midi_mapping.cpp
                           INCLUDE_DIRS include
                           REQUIRES etl audio_param midi rapidjson
                           PRIV_REQUIRES compiler_warning_flags)
else()
    add_library(midi_mapping STATIC)
    target_include_directories(midi_mapping PUBLIC include)
    target_sources(midi_mapping PRIVATE src/midi_mapping.cpp)

    target_link_libraries(midi_mapping PUBLIC etl)
    target_link_libraries(midi_mapping PUBLIC log)
    target_link_libraries(midi_mapping PUBLIC rapidjson)
    target_link_libraries(midi_mapping PUBLIC audio_param_mock)
    target_link_libraries(midi_mapping PUBLIC midi_mock)

    add_executable(midi_mapping_test
                   test/test_midi_mapping.cpp
                   test/test_json_parser.cpp
                   )

    target_link_libraries(midi_mapping_test PRIVATE midi_mapping)

    target_link_libraries(midi_mapping_test PRIVATE gtest_main)
    target_link_libraries(midi_mapping_test PRIVATE gmock)
    target_link_libraries(midi_mapping_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(midi_mapping_test)

    add_executable(json_parser_fuzz
                   src/midi_mapping.cpp
                   test/json_parser_fuzz.cpp
                   )

    target_link_libraries(json_parser_fuzz PRIVATE midi_mapping)


endif()
