idf_component_register(SRCS
                       i2s.cpp
                       process.cpp
                       profiling.cpp
                       INCLUDE_DIRS include
                       REQUIRES driver
                       PRIV_REQUIRES
                       esp-dsp
                       float_convert
                       audio_events
                       audio_param
                       hardware
                       chorus
                       gate
                       valvestate
                       PRIV_REQUIRES compiler_warning_flags)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion -Werror=double-promotion)
