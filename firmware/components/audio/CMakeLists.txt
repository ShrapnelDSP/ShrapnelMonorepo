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
