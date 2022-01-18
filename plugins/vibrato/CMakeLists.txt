cmake_minimum_required(VERSION 3.15)
project(AUDIO_PLUGIN_EXAMPLE VERSION 0.0.1)

add_subdirectory(../JUCE JUCE)
add_subdirectory(../../effects/abstract_dsp abstract_dsp)

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

target_compile_definitions(Vibrato
    PUBLIC
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Vibrato
    PRIVATE
        abstract_dsp
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
