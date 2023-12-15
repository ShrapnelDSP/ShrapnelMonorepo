add_library(idf_nvs_flash INTERFACE)
add_library(idf::nvs_flash ALIAS idf_nvs_flash)

target_include_directories(idf_nvs_flash INTERFACE include)

target_link_libraries(idf_nvs_flash INTERFACE idf::esp_common)