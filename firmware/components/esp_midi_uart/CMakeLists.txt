add_library(shrapnel_esp_midi_uart STATIC)
add_library(shrapnel::esp_midi_uart ALIAS shrapnel_esp_midi_uart)

target_sources(shrapnel_esp_midi_uart PRIVATE src/esp_midi_uart.cpp)

target_include_directories(shrapnel_esp_midi_uart PUBLIC include)

target_link_libraries(shrapnel_esp_midi_uart PUBLIC
                      shrapnel::midi_protocol
                      idf::freertos
                      idf::driver
)