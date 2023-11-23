add_library(presets STATIC)
add_library(shrapnel::presets ALIAS presets)

target_sources(presets
               PRIVATE
               proto/generated/presets.pb-c.c
               src/presets_json_builder.cpp)
target_include_directories(presets PUBLIC proto/generated include)

target_link_libraries(presets PUBLIC idf::protobuf-c shrapnel::rapidjson)