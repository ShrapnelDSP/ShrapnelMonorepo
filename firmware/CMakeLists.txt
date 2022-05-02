cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)

# Use the example Wi-Fi configuration tools
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

list(APPEND EXTRA_COMPONENT_DIRS ../effects)
list(APPEND EXTRA_COMPONENT_DIRS ../thirdparty/esp-idf-components)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32-dsp)
