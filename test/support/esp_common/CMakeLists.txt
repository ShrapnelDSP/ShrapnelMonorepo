add_library(esp_common STATIC)
add_library(idf::esp_common ALIAS esp_common)

target_include_directories(esp_common PUBLIC include)
target_include_directories(esp_common PUBLIC $ENV{IDF_PATH}/components/esp_common/include)

target_sources(esp_common PRIVATE src/esp_err.c)
