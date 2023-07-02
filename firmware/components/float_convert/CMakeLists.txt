add_library(float_convert INTERFACE)
add_library(shrapnel::float_convert ALIAS float_convert)

target_include_directories(float_convert INTERFACE ".")
