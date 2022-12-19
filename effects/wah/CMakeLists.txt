if(ESP_PLATFORM)
idf_component_register(SRCS wah.cpp
                       INCLUDE_DIRS include
                       REQUIRES abstract_dsp
                       PRIV_REQUIRES compiler_warning_flags)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
else()

add_library(wah STATIC
            wah.cpp)

target_include_directories(wah PUBLIC include)

target_link_libraries(wah PUBLIC abstract_dsp)
target_link_libraries(wah PRIVATE compiler_warning_flags)

endif()
