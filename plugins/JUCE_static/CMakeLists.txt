add_library(juce_modules STATIC)

target_link_libraries(juce_modules
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# We're linking the modules privately, but we need to export
# their compile flags
target_compile_definitions(juce_modules
    PUBLIC
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JucePlugin_Build_Standalone=1
        JUCE_STANDALONE_APPLICATION=JucePlugin_Build_Standalone
    INTERFACE
        $<TARGET_PROPERTY:juce_modules,COMPILE_DEFINITIONS>)

# We also need to export the include directories for the modules
target_include_directories(juce_modules
    INTERFACE
        $<TARGET_PROPERTY:juce_modules,INCLUDE_DIRECTORIES>)

# Required when linking VST plugins
set_property(TARGET juce_modules PROPERTY POSITION_INDEPENDENT_CODE ON)
