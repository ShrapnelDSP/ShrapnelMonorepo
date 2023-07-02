add_library(heap_tracing INTERFACE)
add_library(shrapnel::heap_tracing ALIAS heap_tracing)

target_include_directories(heap_tracing INTERFACE include)
