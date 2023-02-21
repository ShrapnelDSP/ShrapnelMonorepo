if(ESP_PLATFORM)
add_library(chorus STATIC)
add_library(shrapnel::chorus ALIAS chorus)

target_sources(chorus PRIVATE src/chorus.cpp)
target_include_directories(chorus PUBLIC include)

target_link_libraries(chorus
                      PUBLIC
                      idf::abstract_dsp
                      PRIVATE
                      shrapnel::compiler_warning_flags)

target_compile_options(chorus PRIVATE -Wdouble-promotion)

else()

add_library(chorus STATIC
            src/chorus.cpp)

target_include_directories(chorus PUBLIC include)

target_link_libraries(chorus PUBLIC abstract_dsp)
target_link_libraries(chorus PRIVATE compiler_warning_flags)

endif()
