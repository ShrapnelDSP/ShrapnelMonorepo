add_library(shrapnel_uuid INTERFACE)
add_library(shrapnel::uuid ALIAS shrapnel_uuid)

target_include_directories(shrapnel_uuid INTERFACE include)
