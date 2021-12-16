cmake_minimum_required(VERSION 3.14)
project (Tutorial
         LANGUAGES C CXX
         )

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/97a467571a0f615a4d96e79e4399c43221ca1232.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
include(CTest)

add_compile_options(-W -Wall -Wextra)

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
