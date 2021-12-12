add_library(freertos src/task.c src/queue.c)
target_include_directories(freertos PUBLIC include)
