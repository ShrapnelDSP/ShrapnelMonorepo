add_library(wah STATIC wah.cpp)
add_library(shrapnel::wah ALIAS wah)

target_include_directories(wah PUBLIC include)

target_link_libraries(wah
                      PUBLIC
                      shrapnel::abstract_dsp
                      PRIVATE
                      shrapnel::compiler_warning_flags)

target_compile_options(wah PRIVATE -Wdouble-promotion)
