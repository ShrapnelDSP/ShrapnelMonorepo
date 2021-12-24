if(ESP_PLATFORM)
idf_component_register(SRCS "abstract_dsp_esp32.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES "esp-dsp")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
else()

add_library(abstract_dsp
            abstract_dsp_juce.cpp)

target_include_directories(abstract_dsp PUBLIC include)

target_link_libraries(abstract_dsp PRIVATE juce::juce_dsp)

# https://forum.juce.com/t/building-my-vst3-with-linux-and-juce6-couple-of-tiny-things/39014
# TODO
# > For SHARED and MODULE libraries the POSITION_INDEPENDENT_CODE
#   target property is set to ON automatically.
#
# Does that mean we building this as a SHARED library?
set_target_properties(abstract_dsp PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)

endif()
