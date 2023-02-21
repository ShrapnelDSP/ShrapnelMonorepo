if (ESP_PLATFORM)
    cmake_minimum_required(VERSION 3.13)

    set(CMAKE_CXX_STANDARD 20)

    option(SHRAPNEL_RESET_WIFI_CREDENTIALS
            "Remove the provisioning Wi-Fi credentials at power up"
            OFF
            )

    if (SHRAPNEL_RESET_WIFI_CREDENTIALS)
        add_compile_definitions(SHRAPNEL_RESET_WIFI_CREDENTIALS=1)
    endif ()

    list(APPEND EXTRA_COMPONENT_DIRS ../effects/abstract_dsp)
    list(APPEND EXTRA_COMPONENT_DIRS ../effects/gate)
    list(APPEND EXTRA_COMPONENT_DIRS ../effects/valvestate)
    list(APPEND EXTRA_COMPONENT_DIRS ../effects/wah)
    list(APPEND EXTRA_COMPONENT_DIRS ../thirdparty/esp-idf-components/esp-dsp)
    list(APPEND EXTRA_COMPONENT_DIRS ../thirdparty/esp-idf-components/rapidjson)
    list(APPEND EXTRA_COMPONENT_DIRS components/audio_events)
    list(APPEND EXTRA_COMPONENT_DIRS components/audio_param)
    list(APPEND EXTRA_COMPONENT_DIRS components/cmd_handling)
    list(APPEND EXTRA_COMPONENT_DIRS components/float_convert)
    list(APPEND EXTRA_COMPONENT_DIRS components/hardware)
    list(APPEND EXTRA_COMPONENT_DIRS components/heap_tracing)
    list(APPEND EXTRA_COMPONENT_DIRS components/i2c)
    list(APPEND EXTRA_COMPONENT_DIRS components/i2s)
    list(APPEND EXTRA_COMPONENT_DIRS components/messages)
    list(APPEND EXTRA_COMPONENT_DIRS components/midi_mapping)
    list(APPEND EXTRA_COMPONENT_DIRS components/midi_protocol)
    list(APPEND EXTRA_COMPONENT_DIRS components/os)
    list(APPEND EXTRA_COMPONENT_DIRS components/pcm3060)
    list(APPEND EXTRA_COMPONENT_DIRS components/persistence)
    list(APPEND EXTRA_COMPONENT_DIRS components/server)
    list(APPEND EXTRA_COMPONENT_DIRS components/wifi)

    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(esp32-dsp)

    add_subdirectory(../effects/chorus chorus)
    add_subdirectory(components/compiler_warning_flags)
    add_subdirectory(components/audio)
    add_subdirectory(../thirdparty/esp-idf-components/etl etl)

    return()
endif()

add_subdirectory(main)
add_subdirectory(components)
