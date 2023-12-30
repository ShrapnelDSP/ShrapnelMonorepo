add_library(esp_persistence STATIC)
add_library(shrapnel::esp_persistence ALIAS esp_persistence)

target_sources(esp_persistence PRIVATE src/esp_persistence.cpp)
target_include_directories(esp_persistence PUBLIC include)

target_link_libraries(esp_persistence
                      PUBLIC
                      shrapnel::etl
                      shrapnel::persistence
                      PRIVATE
                      idf::nvs_flash
                      idf::log
                      shrapnel::compiler_warning_flags)
