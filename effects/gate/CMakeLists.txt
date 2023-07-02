add_library(gate STATIC noise_gate.cpp)
add_library(shrapnel::gate ALIAS gate)

target_include_directories(gate PUBLIC include)

target_link_libraries(gate PUBLIC shrapnel::abstract_dsp)
target_link_libraries(gate PRIVATE shrapnel::compiler_warning_flags)
