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

    list(APPEND EXTRA_COMPONENT_DIRS ../thirdparty/esp-idf-components/esp-dsp)

    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(esp32-dsp)

    add_subdirectory(../effects/abstract_dsp abstract_dsp)
    add_subdirectory(../effects/chorus chorus)
    add_subdirectory(../effects/gate gate)
    add_subdirectory(../effects/valvestate valvestate)
    add_subdirectory(../effects/wah wah)
    add_subdirectory(../thirdparty/esp-idf-components/etl etl)
    add_subdirectory(../thirdparty/esp-idf-components/cppcodec cppcodec)
    add_subdirectory(../thirdparty/esp-idf-components/nanopb nanopb)
    add_subdirectory(components/api)
    add_subdirectory(components/audio)
    add_subdirectory(components/audio_events)
    add_subdirectory(components/audio_param)
    add_subdirectory(components/cmd_handling)
    add_subdirectory(components/compiler_warning_flags)
    add_subdirectory(components/esp_persistence)
    add_subdirectory(components/float_convert)
    add_subdirectory(components/hardware)
    add_subdirectory(components/heap_tracing)
    add_subdirectory(components/i2c)
    add_subdirectory(components/i2s)
    add_subdirectory(components/messages)
    add_subdirectory(components/midi_mapping)
    add_subdirectory(components/midi_protocol)
    add_subdirectory(components/os)
    add_subdirectory(components/pcm3060)
    add_subdirectory(components/persistence)
    add_subdirectory(components/presets)
    add_subdirectory(components/server)
    add_subdirectory(components/wifi)

    return()
endif()

add_subdirectory(main)
add_subdirectory(components)
