add_library(freertos src/task.c src/queue.cpp)
target_include_directories(freertos PUBLIC include)
