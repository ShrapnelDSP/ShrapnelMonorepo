add_library(shrapnel_console STATIC)
add_library(shrapnel::console ALIAS shrapnel_console)

target_sources(shrapnel_console
               PRIVATE
               src/shrapnel_console.cpp)

target_include_directories(shrapnel_console
                           PUBLIC
                           include)

target_link_libraries(shrapnel_console
                      PUBLIC
                      idf::esp_common
                      idf::log
                      shrapnel::embedded_cli
                      PRIVATE
                      shrapnel::compiler_warning_flags)
