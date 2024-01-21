add_library(dummy INTERFACE)
target_link_libraries(dummy INTERFACE )

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    if (CONFIG_IDF_TARGET_ESP32C6 OR CONFIG_IDF_TARGET_ESP32C5 OR CONFIG_IDF_TARGET_ESP32H2)
        set(RUST_TARGET "riscv32imac-esp-espidf")
    else ()
        set(RUST_TARGET "riscv32imc-esp-espidf")
    endif()
elseif(CONFIG_IDF_TARGET_ESP32)
    set(RUST_TARGET "xtensa-esp32-espidf")
elseif(CONFIG_IDF_TARGET_ESP32S2)
    set(RUST_TARGET "xtensa-esp32s2-espidf")
elseif(CONFIG_IDF_TARGET_ESP32S3)
    set(RUST_TARGET "xtensa-esp32s3-espidf")
else()
    message(FATAL_ERROR "Unsupported target ${CONFIG_IDF_TARGET}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_BUILD_TYPE "debug")
    set(CARGO_BUILD_ARG "")
else()
    set(CARGO_BUILD_TYPE "release")
    set(CARGO_BUILD_ARG "--release")
endif()


set(CARGO_BUILD_STD_ARG -Zbuild-std=core,alloc,panic_abort)


if(IDF_VERSION_MAJOR GREATER "4")
set(ESP_RUSTFLAGS "--cfg espidf_time64")
endif()

set(CARGO_PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(CARGO_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(CARGO_TARGET_DIR "${CARGO_BUILD_DIR}/target")

set(RUST_INCLUDE_DIR "${CARGO_TARGET_DIR}")
set(RUST_STATIC_LIBRARY "${CARGO_TARGET_DIR}/${RUST_TARGET}/${CARGO_BUILD_TYPE}/librust_firmware_rust.a")

# if this component uses CBindGen to generate a C header, uncomment the lines below and adjust the header name accordingly
#set(RUST_INCLUDE_HEADER "${RUST_INCLUDE_DIR}/RustApi.h")
#set_source_files_properties("${RUST_INCLUDE_HEADER}" PROPERTIES GENERATED true)

idf_build_get_property(sdkconfig SDKCONFIG)
idf_build_get_property(idf_path IDF_PATH)

ExternalProject_Add(
    project_rust_firmware_rust
    PREFIX "${CARGO_PROJECT_DIR}"
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env
        cargo clean --target ${RUST_TARGET} --target-dir ${CARGO_TARGET_DIR}
    USES_TERMINAL_BUILD true
    BUILD_COMMAND ${CMAKE_COMMAND} -E env
        #CARGO_CMAKE_BUILD_INCLUDES=$<TARGET_PROPERTY:dummy,INTERFACE_INCLUDE_DIRECTORIES>
        #CARGO_CMAKE_BUILD_LINK_LIBRARIES=$<TARGET_PROPERTY:dummy,INTERFACE_LINK_LIBRARIES>
        CARGO_CMAKE_BUILD_SDKCONFIG=${sdkconfig}
        CARGO_CMAKE_BUILD_ESP_IDF=${idf_path}
        CARGO_CMAKE_BUILD_COMPILER=${CMAKE_C_COMPILER}
        RUSTFLAGS=${ESP_RUSTFLAGS}
        MCU=${CONFIG_IDF_TARGET}
        cargo build --target ${RUST_TARGET} --target-dir ${CARGO_TARGET_DIR} ${CARGO_BUILD_ARG} ${CARGO_BUILD_STD_ARG}
    INSTALL_COMMAND ""
    BUILD_ALWAYS TRUE
    TMP_DIR "${CARGO_BUILD_DIR}/tmp"
    STAMP_DIR "${CARGO_BUILD_DIR}/stamp"
    DOWNLOAD_DIR "${CARGO_BUILD_DIR}"
    SOURCE_DIR "${CARGO_PROJECT_DIR}"
    BINARY_DIR "${CARGO_PROJECT_DIR}"
    INSTALL_DIR "${CARGO_BUILD_DIR}"
    BUILD_BYPRODUCTS
        "${RUST_INCLUDE_HEADER}"
        "${RUST_STATIC_LIBRARY}"
)

add_library(library_rust_firmware_rust STATIC IMPORTED)
set_target_properties(library_rust_firmware_rust PROPERTIES IMPORTED_LOCATION ${RUST_STATIC_LIBRARY})
target_link_libraries(library_rust_firmware_rust INTERFACE "idf::pthread" "idf::driver")
add_dependencies(library_rust_firmware_rust project_rust_firmware_rust)
target_include_directories(library_rust_firmware_rust INTERFACE "${RUST_INCLUDE_DIR}")

add_library(rust_firmware_rust INTERFACE)
target_link_libraries(rust_firmware_rust INTERFACE library_rust_firmware_rust)

add_library(shrapnel::rust_firmware ALIAS rust_firmware_rust)
