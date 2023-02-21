if(ESP_PLATFORM)
idf_component_register(SRCS src/valvestate.cpp
                            src/clipping.cpp
                            src/contour.cpp
                            src/fmv.cpp
                            src/gain_control.cpp
                            src/input_filter.cpp
                            src/wave_shape.cpp
                       INCLUDE_DIRS include
                       REQUIRES abstract_dsp)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
target_link_libraries(${COMPONENT_LIB} PRIVATE shrapnel::compiler_warning_flags)
else()

add_library(valvestate STATIC
            src/valvestate.cpp
            src/clipping.cpp
            src/contour.cpp
            src/fmv.cpp
            src/gain_control.cpp
            src/input_filter.cpp
            src/wave_shape.cpp)

target_include_directories(valvestate PUBLIC include)

target_link_libraries(valvestate PUBLIC abstract_dsp)
target_link_libraries(valvestate PRIVATE compiler_warning_flags)

endif()
