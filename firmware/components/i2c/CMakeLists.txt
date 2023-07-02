add_library(i2c STATIC)
add_library(shrapnel::i2c ALIAS i2c)

target_sources(i2c PRIVATE i2c.c)
target_include_directories(i2c PUBLIC include)

target_link_libraries(i2c
                      PRIVATE
                      idf::driver
                      shrapnel::compiler_warning_flags)