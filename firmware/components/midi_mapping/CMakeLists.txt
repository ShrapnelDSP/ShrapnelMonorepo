if(NOT DEFINED TESTING)
    idf_component_register(SRCS src/midi_mapping.cpp
                                src/midi_mapping_api.cpp
                                src/midi_mapping_json_parser.cpp
                                src/midi_mapping_json_builder.cpp
                           INCLUDE_DIRS include
                           REQUIRES etl audio_param midi rapidjson
                           PRIV_REQUIRES compiler_warning_flags)
else()
    add_executable(midi_mapping_test
                   src/midi_mapping.cpp
                   src/midi_mapping_api.cpp
                   src/midi_mapping_json_parser.cpp
                   src/midi_mapping_json_builder.cpp
                   test/test_midi_mapping.cpp
                   test/test_json_parser.cpp
                   test/test_json_builder.cpp
                   )

    target_include_directories(midi_mapping_test PRIVATE include)

    target_link_libraries(midi_mapping_test PRIVATE etl)
    target_link_libraries(midi_mapping_test PRIVATE log)
    target_link_libraries(midi_mapping_test PRIVATE audio_param_mock)
    target_link_libraries(midi_mapping_test PRIVATE midi_mock)
    target_link_libraries(midi_mapping_test PRIVATE rapidjson)

    target_link_libraries(midi_mapping_test PRIVATE gtest_main)
    target_link_libraries(midi_mapping_test PRIVATE gmock)
    target_link_libraries(midi_mapping_test PRIVATE compiler_warning_flags)
    gtest_discover_tests(midi_mapping_test)
endif()
