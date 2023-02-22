add_library(log STATIC src/log.c)
add_library(idf::log ALIAS log)

target_include_directories(log PUBLIC include)
