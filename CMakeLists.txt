idf_component_register(SRCS "abstract_dsp_esp32.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES "esp-dsp")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
