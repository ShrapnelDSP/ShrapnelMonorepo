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
set_target_properties(abstract_dsp PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)

endif()
