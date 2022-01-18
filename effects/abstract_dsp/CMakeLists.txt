if(ESP_PLATFORM)
idf_component_register(SRCS "abstract_dsp_esp32.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES "esp-dsp")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
else()

add_library(abstract_dsp STATIC
            abstract_dsp_juce.cpp)

target_include_directories(abstract_dsp PUBLIC include)

target_link_libraries(abstract_dsp INTERFACE juce::juce_dsp)

target_include_directories(abstract_dsp
    PRIVATE
        $<TARGET_PROPERTY:juce::juce_dsp,INTERFACE_INCLUDE_DIRECTORIES>)

target_compile_definitions(abstract_dsp
    PRIVATE
        $<TARGET_PROPERTY:juce::juce_dsp,INTERFACE_COMPILE_DEFINITIONS>)
endif()
