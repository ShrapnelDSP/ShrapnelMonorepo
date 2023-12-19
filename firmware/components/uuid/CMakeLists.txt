include(../../../thirdparty/esp-idf-components/nanopb/nanopb-generate-cpp.cmake)

add_library(shrapnel_uuid STATIC)
add_library(shrapnel::uuid ALIAS shrapnel_uuid)

set(NANOPB_GENERATE_CPP_APPEND_PATH ON)
nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS ../../../proto/uuid.proto)

target_include_directories(shrapnel_uuid PUBLIC
                           include
                           # for generated proto headers
                           ${CMAKE_CURRENT_BINARY_DIR})

target_sources(shrapnel_uuid PRIVATE
               ${PROTO_SRCS}
               src/uuid.cpp)

target_link_libraries(shrapnel_uuid PUBLIC shrapnel::api shrapnel::nanopb)
