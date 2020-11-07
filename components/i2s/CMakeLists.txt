idf_component_register(SRCS "i2s.c" "process.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver
                       PRIV_REQUIRES "esp-dsp" "float_convert")
