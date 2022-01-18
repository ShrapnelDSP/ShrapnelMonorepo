if(ESP_SYSTEM)
idf_component_register(SRCS "noise_gate.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES "abstract_dsp")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
else()

add_library(gate STATIC
            noise_gate.c)

target_include_directories(gate PUBLIC include)

target_link_libraries(gate PRIVATE abstract_dsp)

endif()
