add_library(shrapnel_embedded_cli)
add_library(shrapnel::embedded_cli ALIAS shrapnel_embedded_cli)

target_include_directories(shrapnel_embedded_cli PUBLIC EmbeddedCLI)
target_sources(shrapnel_embedded_cli PRIVATE EmbeddedCLI/embedded_cli.c)

target_compile_definitions(shrapnel_embedded_cli PUBLIC
                           EMBEDDED_CLI_MAX_LINE=120
                           EMBEDDED_CLI_HISTORY_LEN=0
                           EMBEDDED_CLI_MAX_ARGC=16
                           EMBEDDED_CLI_MAX_PROMPT_LEN=12
                           EMBEDDED_CLI_SERIAL_XLATE=0
)
