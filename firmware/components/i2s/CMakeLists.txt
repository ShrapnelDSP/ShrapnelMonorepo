add_library(i2s STATIC)
add_library(shrapnel::i2s ALIAS i2s)

target_sources(i2s PRIVATE src/i2s.cpp)
target_include_directories(i2s PUBLIC include)

target_link_libraries(i2s
                      PUBLIC
                      idf::driver
                      PRIVATE
                      shrapnel::abstract_dsp
                      shrapnel::compiler_warning_flags
                      shrapnel::audio
                      shrapnel::audio_events
                      shrapnel::audio_param
                      shrapnel::float_convert
                      shrapnel::hardware)

target_compile_options(i2s
                       PRIVATE
                       -Wdouble-promotion
                       -Werror=double-promotion)
