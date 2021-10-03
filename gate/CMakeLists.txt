add_library(gate STATIC noise_gate.c abstract_dsp_juce.cpp)
target_include_directories(gate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
