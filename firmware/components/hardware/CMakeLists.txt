add_library(hardware INTERFACE)
add_library(shrapnel::hardware ALIAS hardware)

target_include_directories(hardware INTERFACE include)