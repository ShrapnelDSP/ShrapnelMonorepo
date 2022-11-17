if(NOT DEFINED TESTING)
idf_component_register()

add_subdirectory(etl)

target_link_libraries(${COMPONENT_LIB} INTERFACE etl)
target_include_directories(${COMPONENT_LIB} INTERFACE include)
else()
add_subdirectory(etl)
target_include_directories(etl INTERFACE include)
endif()
