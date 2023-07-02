add_library(esp_hw_support INTERFACE)
add_library(idf::esp_hw_support ALIAS esp_hw_support)

target_include_directories(esp_hw_support INTERFACE include)
