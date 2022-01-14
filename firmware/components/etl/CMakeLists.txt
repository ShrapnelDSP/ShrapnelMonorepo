if(NOT DEFINED TESTING)
idf_component_register()

add_subdirectory(etl)

target_link_libraries(${COMPONENT_LIB} INTERFACE etl)
else()
add_subdirectory(etl)
endif()
