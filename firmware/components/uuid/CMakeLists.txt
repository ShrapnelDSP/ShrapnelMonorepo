add_library(shrapnel_uuid STATIC)
add_library(shrapnel::uuid ALIAS shrapnel_uuid)

target_include_directories(shrapnel_uuid PUBLIC include)

target_sources(shrapnel_uuid PRIVATE src/uuid_json.cpp)

target_link_libraries(shrapnel_uuid
                      PRIVATE
                      idf::log
                      shrapnel::compiler_warning_flags
                      PUBLIC
                      shrapnel::rapidjson)
