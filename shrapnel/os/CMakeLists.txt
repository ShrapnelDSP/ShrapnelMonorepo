add_library(os STATIC)
add_library(shrapnel::os ALIAS os)

target_sources(os PRIVATE universal/timer.cpp)
target_include_directories(os PUBLIC include)

target_link_libraries(os
                      PUBLIC
                      shrapnel::etl
                      idf::freertos)

if(ESP_PLATFORM)
    target_sources(os PRIVATE esp32/timer_impl.cpp universal/timer.cpp esp32/debug.cpp)
    target_include_directories(os PRIVATE esp32)

    return()
endif()

target_include_directories(os PUBLIC desktop)

target_sources(os PRIVATE desktop/timer_impl.cpp)
