cmake_minimum_required(VERSION 3.15)
project(AUDIO_PLUGIN_EXAMPLE VERSION 0.0.1)

add_subdirectory(../JUCE JUCE)
add_subdirectory(../JUCE_static JUCE_static)

# Required when linking VST plugins
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(../../effects/abstract_dsp abstract_dsp)

juce_add_plugin(Chorus
    COMPANY_NAME "Barabas Raffai"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE BaRa
    PLUGIN_CODE ShCh
    FORMATS Standalone
    PRODUCT_NAME "Shrapnel Chorus")

target_sources(Chorus
    PRIVATE
        PluginProcessor.cpp)

target_link_libraries(Chorus
    PRIVATE
        abstract_dsp
        juce_modules)
