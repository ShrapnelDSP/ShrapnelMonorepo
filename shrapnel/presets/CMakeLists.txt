include(../../thirdparty/esp-idf-components/nanopb/nanopb-generate-cpp.cmake)

add_library(presets STATIC)
add_library(shrapnel::presets ALIAS presets)

set(NANOPB_GENERATE_CPP_APPEND_PATH ON)
nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS
                    ../../proto/presets.proto
                    ../../proto/selected_preset.proto)

target_sources(presets
               PRIVATE
               ${PROTO_SRCS}
               src/preset_manager.cpp
               src/presets_api.cpp
               src/presets_to_string.cpp
               src/selected_preset_api.cpp
               src/selected_preset_manager.cpp
               src/selected_preset_to_string.cpp)
               
target_include_directories(presets PUBLIC
                           include
                           # for generated proto headers
                           ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(presets
                      PUBLIC
                      idf::log
                      shrapnel::api
                      shrapnel::etl
                      shrapnel::persistence
                      shrapnel::nanopb)
