add_library(idf_protobuf-c STATIC)
add_library(idf::protobuf-c ALIAS idf_protobuf-c)

target_sources(idf_protobuf-c PRIVATE protobuf-c/protobuf-c/protobuf-c.c)
target_include_directories(idf_protobuf-c PUBLIC protobuf-c)
