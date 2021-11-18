idf_component_register(SRCS "noise_gate.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES "abstract_dsp")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
