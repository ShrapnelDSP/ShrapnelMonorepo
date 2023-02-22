add_library(esp_common INTERFACE)
add_library(idf::esp_common ALIAS esp_common)

target_include_directories(esp_common INTERFACE $ENV{IDF_PATH}/components/esp_common/include)
target_include_directories(esp_common INTERFACE include)
