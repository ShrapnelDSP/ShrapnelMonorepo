cmake_minimum_required(VERSION 3.22)
cmake_policy(VERSION 3.22)
project (ShrapnelDSPTest LANGUAGES C CXX)

# This project uses C++20 features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

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

add_compile_definitions(TESTING)
add_compile_definitions(LOG_DEFAULT_LEVEL=ESP_LOG_VERBOSE)

# Tells the component cmake files to become test components instead of esp-idf
# components
set(TESTING ON)

add_subdirectory(support)

add_subdirectory(../effects/abstract_dsp/ abstract_dsp)
add_subdirectory(../plugins/JUCE_static JUCE_static)
add_subdirectory(../firmware firmware)
add_subdirectory(../thirdparty thirdparty)
add_subdirectory(integration)
