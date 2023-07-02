add_library(wifi STATIC)
add_library(shrapnel::wifi ALIAS wifi)

target_sources(wifi PRIVATE src/wifi_state_machine.cpp)
target_include_directories(wifi PUBLIC include)

target_link_libraries(wifi
                      PUBLIC
                      idf::wifi_provisioning
                      shrapnel::os
                      idf::nvs_flash
                      shrapnel::etl
                      PRIVATE
                      shrapnel::compiler_warning_flags)
