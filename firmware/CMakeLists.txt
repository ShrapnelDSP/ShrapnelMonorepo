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

    list(APPEND EXTRA_COMPONENT_DIRS ../effects)
    list(APPEND EXTRA_COMPONENT_DIRS ../thirdparty/esp-idf-components)

    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(esp32-dsp)
    return()
endif()

add_subdirectory(main)
add_subdirectory(components)
