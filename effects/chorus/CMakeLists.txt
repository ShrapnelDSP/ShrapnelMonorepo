if(ESP_PLATFORM)
idf_component_register(SRCS src/chorus.cpp
                       INCLUDE_DIRS include
                       REQUIRES abstract_dsp
                       PRIV_REQUIRES compiler_warning_flags)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
else()

add_library(chorus STATIC
            src/chorus.cpp)

target_include_directories(chorus PUBLIC include)

target_link_libraries(chorus PUBLIC abstract_dsp)

endif()
