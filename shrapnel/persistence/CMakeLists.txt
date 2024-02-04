if(NOT DEFINED TESTING)
    add_library(persistence INTERFACE)
    add_library(shrapnel::persistence ALIAS persistence)

    target_include_directories(persistence INTERFACE include)

    target_link_libraries(persistence INTERFACE shrapnel::etl)
else()
    add_library(persistence INTERFACE)
    add_library(shrapnel::persistence ALIAS persistence)

    target_include_directories(persistence INTERFACE include test/include)

    target_link_libraries(persistence INTERFACE shrapnel::etl)
endif()
