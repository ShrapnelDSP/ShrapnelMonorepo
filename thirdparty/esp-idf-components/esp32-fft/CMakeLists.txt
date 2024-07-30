add_library(esp32_fft STATIC)
add_library(shrapnel::esp32_fft ALIAS esp32_fft)

target_sources(esp32_fft PRIVATE esp32-fft/components/esp32_fft/esp32_fft.c)
target_include_directories(esp32_fft PUBLIC esp32-fft/components/esp32_fft/include)
target_link_libraries(esp32_fft PRIVATE shrapnel::compiler_warning_flags)

