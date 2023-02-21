if(ESP_PLATFORM)
    add_library(audio STATIC)
    add_library(shrapnel::audio ALIAS audio)

    target_sources(audio PRIVATE src/audio_processor.cpp)
    target_include_directories(audio PUBLIC include)
    target_link_libraries(audio
                          PUBLIC
                          shrapnel::valvestate
                          shrapnel::wah
                          shrapnel::chorus
                          shrapnel::gate
                          PRIVATE
                          shrapnel::abstract_dsp
                          shrapnel::compiler_warning_flags)

    target_compile_options(audio PRIVATE -Wdouble-promotion -Werror=double-promotion)
    return()
endif()

add_library(audio STATIC)
target_sources(audio PRIVATE src/audio_processor.cpp)
target_include_directories(audio PUBLIC include)
target_link_libraries(audio PUBLIC chorus gate valvestate wah)
target_link_libraries(audio PRIVATE abstract_dsp compiler_warning_flags)
