cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)

option(SHRAPNEL_RESET_WIFI_CREDENTIALS
       "Remove the provisioning Wi-Fi credentials at power up"
       OFF
)

if(SHRAPNEL_RESET_WIFI_CREDENTIALS)
    add_compile_definitions(SHRAPNEL_RESET_WIFI_CREDENTIALS=1)
endif()

list(APPEND EXTRA_COMPONENT_DIRS ../thirdparty/esp-idf-components/esp-dsp)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32-dsp)

add_subdirectory(../effects effects)
add_subdirectory(../shrapnel shrapnel)
add_subdirectory(../thirdparty/esp-idf-components/etl etl)
add_subdirectory(../thirdparty/esp-idf-components/nanopb nanopb)
add_subdirectory(components/esp_midi_uart)
add_subdirectory(components/esp_persistence)
add_subdirectory(components/hardware)
add_subdirectory(components/heap_tracing)
add_subdirectory(components/i2c)
add_subdirectory(components/i2s)
add_subdirectory(components/pcm3060)
add_subdirectory(components/server)
add_subdirectory(components/wifi)
