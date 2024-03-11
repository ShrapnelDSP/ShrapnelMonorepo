cmake_minimum_required(VERSION 3.15)

project(ShrapnelDSP VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(../../effects/abstract_dsp abstract_dsp)
add_subdirectory(../../effects/chorus chorus)
add_subdirectory(../../effects/gate gate)
add_subdirectory(../../effects/valvestate valvestate)
add_subdirectory(../../effects/wah wah)
add_subdirectory(../../firmware/components/audio audio)
add_subdirectory(../../firmware/components/compiler_warning_flags compiler_warning_flags)
add_subdirectory(../../test/support/esp_common esp_common)
add_subdirectory(../../test/support/esp_hw_support esp_hw_support)
add_subdirectory(../../test/support/log log)
add_subdirectory(../../thirdparty/JUCE JUCE)
add_subdirectory(../../thirdparty/esp-idf-components/esp-dsp esp-dsp)
add_subdirectory(../../thirdparty/esp-idf-components/esp32-fft esp32-fft)
add_subdirectory(../JUCE_static JUCE_static)

juce_add_plugin(ShrapnelDSP
                COMPANY_NAME Barabas Raffai
                PLUGIN_MANUFACTURER_CODE BaRa
                PLUGIN_CODE Shra
                FORMATS VST3 Standalone
                PRODUCT_NAME ShrapnelDSP)

target_sources(ShrapnelDSP
               PRIVATE
               PluginProcessor.cpp)

target_link_libraries(ShrapnelDSP
                      PRIVATE
                      audio
                      juce_modules)
