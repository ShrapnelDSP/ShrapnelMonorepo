add_library(pcm3060 STATIC)
add_library(shrapnel::pcm3060 ALIAS pcm3060)

target_sources(pcm3060 PRIVATE pcm3060.c)
target_include_directories(pcm3060 PUBLIC include)

target_link_libraries(pcm3060
                      PUBLIC
                      idf::driver
                      PRIVATE
                      shrapnel::compiler_warning_flags
                      shrapnel::i2c)
