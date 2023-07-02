add_library(rapidjson INTERFACE)
add_library(shrapnel::rapidjson ALIAS rapidjson)

target_include_directories(rapidjson INTERFACE rapidjson/include)

if(ESP_PLATFORM)
    target_compile_definitions(rapidjson INTERFACE RAPIDJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY=512)
endif()
