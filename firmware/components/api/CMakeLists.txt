add_library(shrapnel_api INTERFACE)
add_library(shrapnel::api ALIAS shrapnel_api)

target_include_directories(shrapnel_api INTERFACE include)
