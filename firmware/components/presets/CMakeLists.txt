add_library(presets STATIC)
add_library(shrapnel::presets ALIAS presets)

target_sources(presets
               PRIVATE
               proto/generated/presets.pb-c.c
               src/presets_json_builder.cpp
               src/selected_preset_json_builder.cpp
               src/selected_preset_json_parser.cpp)
target_include_directories(presets PUBLIC proto/generated include)

target_link_libraries(presets
                      PUBLIC
                      idf::log
                      idf::protobuf-c
                      shrapnel::rapidjson
                      shrapnel::etl
                      shrapnel::uuid
                      PRIVATE
                      cppcodec)

if(DEFINED TESTING)
    add_executable(presets_test
                   test/presets_json_builder.cpp
                   test/selected_preset_json_builder.cpp
                   test/selected_preset_json_parser.cpp)

    target_include_directories(presets_test PRIVATE include)

    target_link_libraries(presets_test
                          PRIVATE
                          presets
                          GTest::gtest_main
                          GTest::gmock
                          shrapnel::compiler_warning_flags)
    gtest_discover_tests(presets_test)
endif()