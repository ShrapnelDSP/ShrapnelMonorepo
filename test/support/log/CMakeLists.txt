add_library(log STATIC src/log.c)
add_library(shrapnel::log ALIAS log)
target_include_directories(log PUBLIC include)
