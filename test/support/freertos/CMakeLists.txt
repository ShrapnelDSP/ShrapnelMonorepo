add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE include)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

add_subdirectory(FreeRTOS-Kernel)
add_library(idf::freertos ALIAS freertos_kernel)