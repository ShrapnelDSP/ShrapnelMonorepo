idf_component_register(SRCS
                       i2s.cpp
                       process.cpp
                       fmv.c
                       input_filter.c
                       profiling.cpp
                       INCLUDE_DIRS include
                       REQUIRES driver
                       PRIV_REQUIRES
                       esp-dsp
                       float_convert
                       iir
                       audio_events
                       audio_param
                       hardware
                       chorus
                       gate)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion -Werror=double-promotion)
