if(NOT DEFINED TESTING)
    add_library(persistence STATIC)
    add_library(shrapnel::persistence ALIAS persistence)

    target_sources(persistence PRIVATE src/esp_persistence.cpp)
    target_include_directories(persistence PUBLIC include)

    target_link_libraries(persistence
                          PUBLIC
                          shrapnel::etl
                          PRIVATE
                          idf::nvs_flash
                          shrapnel::compiler_warning_flags)
else()
    add_library(persistence INTERFACE)
    add_library(shrapnel::persistence ALIAS persistence)

    target_include_directories(persistence INTERFACE include test/include)

    target_link_libraries(persistence INTERFACE shrapnel::etl)
endif()
