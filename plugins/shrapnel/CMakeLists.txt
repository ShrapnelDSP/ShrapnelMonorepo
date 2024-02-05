cmake_minimum_required(VERSION 3.15)

project(ShrapnelDSP VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(../../effects effects)
add_subdirectory(../../shrapnel shrapnel)
add_subdirectory(../../thirdparty/JUCE JUCE)
add_subdirectory(../JUCE_static JUCE_static)
add_subdirectory(../../thirdparty/esp-idf-components/nanopb nanopb)
add_subdirectory(../../thirdparty/esp-idf-components/etl etl)

# Fake esp-idf components are required for esp-dsp to build
add_subdirectory(../../test/support/esp_common esp_common)
add_subdirectory(../../test/support/esp_hw_support esp_hw_support)
add_subdirectory(../../test/support/log log)
add_subdirectory(../../thirdparty/esp-idf-components/esp-dsp esp-dsp)

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
        shrapnel::audio
        shrapnel::main_thread
        juce_modules)
