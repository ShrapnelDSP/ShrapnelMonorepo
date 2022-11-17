cmake_minimum_required(VERSION 3.14)
project (Tutorial
         LANGUAGES C CXX
         )

# This project uses C++20 features
set(CMAKE_CXX_STANDARD 20)

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
add_subdirectory(../firmware/components/midi midi)
add_subdirectory(../firmware/components/midi_mapping midi_mapping)
add_subdirectory(../thirdparty/esp-idf-components/etl etl)
add_subdirectory(../firmware/components/queue queue)
add_subdirectory(../firmware/components/task task)
add_subdirectory(../firmware/components/compiler_warning_flags compiler_warning_flags)
add_subdirectory(../thirdparty/esp-idf-components/esp-dsp esp_dsp)
add_subdirectory(../thirdparty/esp-idf-components/rapidjson rapidjson)
