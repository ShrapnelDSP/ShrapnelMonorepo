if(ESP_PLATFORM)
idf_component_register(SRCS wah.cpp
                       INCLUDE_DIRS include
                       REQUIRES abstract_dsp)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
target_link_libraries(${COMPONENT_LIB} PRIVATE shrapnel::compiler_warning_flags)
else()

add_library(wah STATIC
            wah.cpp)

target_include_directories(wah PUBLIC include)

target_link_libraries(wah PUBLIC abstract_dsp)
target_link_libraries(wah PRIVATE compiler_warning_flags)

endif()
