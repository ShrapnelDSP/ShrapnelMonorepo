idf_component_register(SRCS "i2s.c" "process.cpp" "fmv.c" "input_filter.c" "noise_gate.c" "profiling.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver
                       PRIV_REQUIRES "esp-dsp" "float_convert" "iir" "audio_events" "hardware")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion -Werror=double-promotion)
