add_subdirectory(etl)
add_library(shrapnel::etl ALIAS etl)

target_include_directories(etl INTERFACE include)
