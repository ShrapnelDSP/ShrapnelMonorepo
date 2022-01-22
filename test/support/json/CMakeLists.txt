add_library(json $ENV{IDF_PATH}/components/json/cJSON/cJSON.c)
target_include_directories(json PUBLIC $ENV{IDF_PATH}/components/json/cJSON)
