if(ESP_PLATFORM)
    idf_component_register(SRCS
            esp32/timer_impl.cpp
            universal/timer.cpp
            INCLUDE_DIRS include
            PRIV_INCLUDE_DIRS esp32
            REQUIRES etl
            PRIV_REQUIRES compiler_warning_flags)
    return()
endif()

add_library(os STATIC)
add_library(shrapnel::os ALIAS os)

target_include_directories(os
        PUBLIC
        include
        desktop)

target_sources(os
        PRIVATE
        desktop/timer_impl.cpp
        universal/timer.cpp)

target_link_libraries(os
        PUBLIC
        shrapnel::etl
        shrapnel::freertos)
