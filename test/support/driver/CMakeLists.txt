add_library(driver INTERFACE)
add_library(idf::driver ALIAS driver)

target_include_directories(driver INTERFACE include)
