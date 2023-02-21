add_library(server STATIC)
add_library(shrapnel::server ALIAS server)

target_sources(server PRIVATE src/server.cpp)
target_include_directories(server PUBLIC include)

target_link_libraries(server
                      PUBLIC
                      idf::esp_http_server
                      idf::os
                      shrapnel::messages
                      PRIVATE
                      shrapnel::compiler_warning_flags)
