if(ESP_PLATFORM)
idf_component_register(SRCS noise_gate.cpp
                       INCLUDE_DIRS include
                       REQUIRES abstract_dsp
                       PRIV_REQUIRES compiler_warning_flags)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
else()

add_library(gate STATIC
            noise_gate.cpp)

target_include_directories(gate PUBLIC include)

target_link_libraries(gate PUBLIC abstract_dsp)
target_link_libraries(gate PRIVATE compiler_warning_flags)

endif()
