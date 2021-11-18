cmake_minimum_required(VERSION 3.15)
project(AUDIO_PLUGIN_EXAMPLE VERSION 0.0.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(JUCE)

juce_add_plugin(AudioPluginExample
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

target_include_directories(AudioPluginExample
    PUBLIC
        dsp/include)

target_sources(AudioPluginExample
    PRIVATE
        PluginProcessor.cpp
        dsp/abstract_dsp_juce.cpp)

target_compile_definitions(AudioPluginExample
    PUBLIC
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(AudioPluginExample
    PRIVATE
        juce::juce_dsp
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
