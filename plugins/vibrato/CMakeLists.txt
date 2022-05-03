cmake_minimum_required(VERSION 3.15)
project(AUDIO_PLUGIN_EXAMPLE VERSION 0.0.1)

# Required when linking VST plugins
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(../../thirdparty/JUCE JUCE)
add_subdirectory(../JUCE_static JUCE_static)
add_subdirectory(../../effects/abstract_dsp abstract_dsp)

add_subdirectory(../../firmware/components/compiler_warning_flags compiler_warning_flags)

juce_add_plugin(Vibrato
    COMPANY_NAME "Barabas Raffai"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE BaRa
    PLUGIN_CODE ShVb
    FORMATS VST3 Standalone
    PRODUCT_NAME "Shrapnel Vibrato")

target_sources(Vibrato
    PRIVATE
        PluginProcessor.cpp)

target_link_libraries(Vibrato
    PRIVATE
        abstract_dsp
        juce_modules)
