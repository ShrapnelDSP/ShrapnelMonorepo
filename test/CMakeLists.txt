cmake_minimum_required(VERSION 3.14)
project (Tutorial
         LANGUAGES C CXX
         )

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 17)

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

# TODO this should be added only for our code, it is making library code fail
# to build, so disabled for now. JUCE does this by providing a target that can
# be linked which provides these flags. Then any code that wants to use the
# flags can link that target.
#add_compile_options(-W -Wall -Wextra)
add_compile_definitions(TESTING)

# Tells the component cmake files to become test components instead of esp-idf
# components
set(TESTING ON)

add_subdirectory(support/log)
add_subdirectory(support/freertos)
add_subdirectory(support/esp_common)
add_subdirectory(support/esp_hw_support)
add_subdirectory(support/json)

add_subdirectory(../effects/abstract_dsp/ abstract_dsp)
add_subdirectory(../firmware/components/audio_events audio_events)
add_subdirectory(../firmware/components/audio_param audio_param)
add_subdirectory(../firmware/components/cmd_handling cmd_handling)
add_subdirectory(../thirdparty/esp-idf-components/etl etl)
add_subdirectory(../firmware/components/queue queue)
add_subdirectory(../firmware/components/task task)
add_subdirectory(../thirdparty/esp-idf-components/esp-dsp esp_dsp)
