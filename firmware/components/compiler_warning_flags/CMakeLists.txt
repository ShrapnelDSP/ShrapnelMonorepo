# Based on JUCE's recommended warning flags
# https://github.com/juce-framework/JUCE/blob/6.1.6/extras/Build/CMake/JUCEHelperTargets.cmake

add_library(compiler_warning_flags INTERFACE)

if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    target_compile_options(compiler_warning_flags INTERFACE "/W4")
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    target_compile_options(compiler_warning_flags INTERFACE
        -Wall -Wshadow-all -Wshorten-64-to-32 -Wstrict-aliasing
        -Wuninitialized -Wunused-parameter -Wconversion -Wsign-compare
        -Wint-conversion -Wconditional-uninitialized -Wconstant-conversion
        -Wsign-conversion -Wbool-conversion -Wextra-semi -Wunreachable-code
        -Wcast-align -Wshift-sign-overflow -Wno-missing-field-initializers
        -Wnullable-to-nonnull-conversion -Wno-ignored-qualifiers -Wswitch-enum
        -Wpedantic -Wdeprecated
        $<$<OR:$<COMPILE_LANGUAGE:CXX>,$<COMPILE_LANGUAGE:OBJCXX>>:
            -Wzero-as-null-pointer-constant -Wunused-private-field
            -Woverloaded-virtual -Wreorder
            -Winconsistent-missing-destructor-override>)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(compiler_warning_flags INTERFACE
        -Wall -Wextra -Wpedantic -Wstrict-aliasing -Wuninitialized
        -Wunused-parameter -Wsign-compare -Wsign-conversion -Wunreachable-code
        -Wcast-align -Wno-implicit-fallthrough -Wno-maybe-uninitialized
        -Wno-missing-field-initializers -Wno-ignored-qualifiers -Wswitch-enum
        -Wredundant-decls -Wno-strict-overflow -Wshadow
        $<$<COMPILE_LANGUAGE:CXX>:
            -Woverloaded-virtual -Wreorder -Wzero-as-null-pointer-constant>)
endif()
