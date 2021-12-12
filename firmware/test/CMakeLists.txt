cmake_minimum_required(VERSION 3.6)
project (Tutorial
         LANGUAGES C CXX
         )
include(CTest)
find_package(PkgConfig REQUIRED)

add_compile_options(-W -Wall -Wextra)

pkg_search_module(cpputest REQUIRED IMPORTED_TARGET cpputest)

# Tells the component cmake files to become test components instead of esp-idf
# components
set(TESTING ON)

add_subdirectory(support/log)
add_subdirectory(support/freertos)
add_subdirectory(support/esp_common)
add_subdirectory(support/json)

add_subdirectory(../components/cmd_handling cmd_handling)
add_subdirectory(../components/audio_param audio_param)
add_subdirectory(../components/task task)
