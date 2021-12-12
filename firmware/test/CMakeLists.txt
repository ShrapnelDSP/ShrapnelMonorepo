cmake_minimum_required(VERSION 3.5)
project (Tutorial
         LANGUAGES C CXX
         )
include(CTest)

# Check for CppUTest and bail out if they don't have it
#if(DEFINED ENV{CPPUTEST_HOME})
#  message("Using CppUTest found in $ENV{CPPUTEST_HOME}")
#else()
#  message(FATAL_ERROR "CPPUTEST_HOME is not set; You must tell CMake where to find CppUTest")
#endif()

# Tells the component cmake files to become test components instead of esp-idf
# components
set(TESTING ON)

include_directories(support/freertos/include)
include_directories(support/log/include)

add_subdirectory(../components/cmd_handling cmd_handling)
