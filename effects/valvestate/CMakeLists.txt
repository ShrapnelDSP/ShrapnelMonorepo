if(ESP_PLATFORM)
idf_component_register(SRCS src/valvestate.cpp
                       INCLUDE_DIRS include
                       REQUIRES abstract_dsp)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
else()

add_library(valvestate STATIC
            src/valvestate.cpp
            src/Clipping.cpp
            src/Contour.cpp
            src/FMV.cpp
            src/GainControl.cpp
            src/InputFilter.cpp
            src/WaveShape.cpp)

target_include_directories(valvestate PUBLIC include)

target_link_libraries(valvestate PUBLIC abstract_dsp)

endif()
