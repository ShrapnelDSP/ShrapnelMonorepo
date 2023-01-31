if(ESP_PLATFORM)
    idf_component_register(SRCS
            src/audio_processor.cpp
            INCLUDE_DIRS include
            REQUIRES
            chorus
            gate
            valvestate
            wah
            PRIV_REQUIRES
            abstract_dsp
            compiler_warning_flags)

    target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion -Werror=double-promotion)
    return()
endif()

add_library(audio STATIC)
target_sources(audio PRIVATE src/audio_processor.cpp)
target_include_directories(audio PUBLIC include)
target_link_libraries(audio PUBLIC chorus gate valvestate wah)
target_link_libraries(audio PRIVATE abstract_dsp compiler_warning_flags)
