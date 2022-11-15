if(NOT DEFINED TESTING)
idf_component_register(INCLUDE_DIRS rapidjson/include)

target_compile_definitions(${COMPONENT_LIB} INTERFACE RAPIDJSON_ALLOCATOR_DEFAULT_CHUNK_CAPACITY=512)
else()
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE rapidjson/include)
endif()
