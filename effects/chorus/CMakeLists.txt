if(ESP_SYSTEM)
idf_component_register(SRCS src/chorus.cpp
                       INCLUDE_DIRS include
                       PRIV_REQUIRES abstract_dsp)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wdouble-promotion)
else()

add_library(chorus STATIC
            src/chorus.cpp)

target_include_directories(chorus PUBLIC include)

target_link_libraries(chorus PUBLIC abstract_dsp)

endif()
