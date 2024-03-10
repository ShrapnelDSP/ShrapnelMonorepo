add_library(esp32_dsp STATIC)
add_library(shrapnel::esp32_dsp ALIAS esp32_dsp)

target_sources(esp32_dsp PRIVATE esp32-fft/components/esp32_fft/esp32_fft.c)
target_include_directories(esp32_dsp PUBLIC esp32-fft/components/esp32_fft/include)
target_link_libraries(esp32_dsp PRIVATE shrapnel::compiler_warning_flags)

