cmake_minimum_required(VERSION 3.15)
project(Valvestate VERSION 0.0.1)

# Required when linking VST plugins
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(../JUCE JUCE)
add_subdirectory(../JUCE_static JUCE_static)

add_subdirectory(../../effects/abstract_dsp abstract_dsp)
add_subdirectory(../../effects/valvestate valvestate)

juce_add_plugin(Valvestate
    COMPANY_NAME "Barabas Raffai"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE BaRa
    PLUGIN_CODE ShVS
    FORMATS VST3 Standalone
    PRODUCT_NAME "Shrapnel Valvestate")

target_sources(Valvestate
    PRIVATE
        src/PluginEditor.cpp
        src/PluginProcessor.cpp)

target_include_directories(Valvestate
    PRIVATE
        Source)

target_link_libraries(Valvestate
    PRIVATE
        valvestate
        juce_modules)
