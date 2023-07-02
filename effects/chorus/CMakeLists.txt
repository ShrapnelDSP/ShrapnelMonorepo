add_library(chorus STATIC)
add_library(shrapnel::chorus ALIAS chorus)

target_sources(chorus PRIVATE src/chorus.cpp)
target_include_directories(chorus PUBLIC include)

target_link_libraries(chorus
                      PUBLIC
                      shrapnel::abstract_dsp
                      PRIVATE
                      shrapnel::compiler_warning_flags)

target_compile_options(chorus PRIVATE -Wdouble-promotion)
