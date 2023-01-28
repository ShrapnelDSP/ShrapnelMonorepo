idf_component_register(SRCS
                       audio_processor.cpp
                       i2s.cpp
                       INCLUDE_DIRS include
                       REQUIRES driver
                       PRIV_REQUIRES
                       abstract_dsp
                       esp-dsp
                       float_convert
                       audio_events
                       audio_param
                       hardware
                       chorus
                       gate
                       valvestate
                       wah
                       PRIV_REQUIRES compiler_warning_flags)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion -Werror=double-promotion)