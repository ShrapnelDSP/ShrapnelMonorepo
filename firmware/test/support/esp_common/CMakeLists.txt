add_library(esp_common INTERFACE)
target_include_directories(esp_common INTERFACE $ENV{IDF_PATH}/components/esp_common/include)
