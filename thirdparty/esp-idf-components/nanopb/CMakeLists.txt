# NO_DEFAULT_PATH so that system wide installation of nanopb will be used. We
# must use the submoduled version.
# set(NANOPB_EXTRA_PATH_REL nanopb/extra)
# cmake_path(ABSOLUTE_PATH NANOPB_EXTRA_PATH_REL OUTPUT_VARIABLE NANOPB_EXTRA_PATH)
# list(APPEND CMAKE_MODULE_PATH ${NANOPB_EXTRA_PATH})
# find_package(Nanopb 0.4.9 EXACT MODULE REQUIRED)
 list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
include(nanopb/extra/FindNanopb.cmake)

add_library(shrapnel_nanopb STATIC)
add_library(shrapnel::nanopb ALIAS shrapnel_nanopb)

target_sources(shrapnel_nanopb PRIVATE ${NANOPB_SRCS})
target_include_directories(shrapnel_nanopb PUBLIC ${NANOPB_INCLUDE_DIRS})

