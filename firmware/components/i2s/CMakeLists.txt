if(ESP_PLATFORM)
idf_component_register(SRCS src/i2s.cpp
                       INCLUDE_DIRS include
                       REQUIRES driver
                       PRIV_REQUIRES
                       abstract_dsp
                       )

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion -Werror=double-promotion)
target_link_libraries(${COMPONENT_LIB}
                      PRIVATE
                      shrapnel::compiler_warning_flags
                      shrapnel::audio
                      shrapnel::audio_events
                      shrapnel::audio_param
                      shrapnel::float_convert
                      shrapnel::hardware)
endif()