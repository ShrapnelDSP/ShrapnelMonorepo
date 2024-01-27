add_library(valvestate STATIC
            src/valvestate.cpp
            src/clipping.cpp
            src/contour.cpp
            src/fmv.cpp
            src/gain_control.cpp
            src/input_filter.cpp)
add_library(shrapnel::valvestate ALIAS valvestate)

target_include_directories(valvestate PUBLIC include)

target_link_libraries(valvestate
                      PUBLIC
                      shrapnel::abstract_dsp
                      PRIVATE
                      shrapnel::compiler_warning_flags)

target_compile_options(valvestate PRIVATE -Wdouble-promotion)
