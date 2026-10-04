cmake_minimum_required(VERSION 3.31)
project(zstd C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

cmake_path(SET THIRDPARTY_DIR NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty")
cmake_path(SET ZSTD_LIB_DIR   NORMALIZE "${THIRDPARTY_DIR}/zstd/lib")

# NOTE: The asset tools compress archive entries offline, so the runtime library
# contains only the decoder. Compression, dictionary building, multithreading,
# and legacy formats are left out.
add_library(zstd STATIC
    ${ZSTD_LIB_DIR}/zstd.h
    ${ZSTD_LIB_DIR}/zstd_errors.h
    ${ZSTD_LIB_DIR}/common/debug.c
    ${ZSTD_LIB_DIR}/common/entropy_common.c
    ${ZSTD_LIB_DIR}/common/error_private.c
    ${ZSTD_LIB_DIR}/common/fse_decompress.c
    ${ZSTD_LIB_DIR}/common/xxhash.c
    ${ZSTD_LIB_DIR}/common/zstd_common.c
    ${ZSTD_LIB_DIR}/decompress/huf_decompress.c
    ${ZSTD_LIB_DIR}/decompress/zstd_ddict.c
    ${ZSTD_LIB_DIR}/decompress/zstd_decompress.c
    ${ZSTD_LIB_DIR}/decompress/zstd_decompress_block.c
)

target_include_directories(zstd
    PUBLIC
        ${ZSTD_LIB_DIR}
)

target_compile_definitions(zstd PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        WIN32
        _WINDOWS
    >

    # NOTE: The x86-64 assembly Huffman decoder needs an assembler setup for
    # each CMake generator. Use the C implementation on every platform.
    ZSTD_DISABLE_ASM
)

target_compile_options(zstd PRIVATE
    # NOTE: GNU-style frontend (Clang, Emscripten, Apple Clang, and GCC; excludes clang-cl).
    $<$<CXX_COMPILER_FRONTEND_VARIANT:GNU>:
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>
    >
    # NOTE: MSVC-style frontend (MSVC and clang-cl).
    $<$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>:
        /nologo
        /W3
        /O2
        /Ob1
        /GF
        /Gy
        /GS-
        /utf-8
        /MP # NOTE: Build with multiple processes for faster builds
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    >
    $<$<AND:$<PLATFORM_ID:Emscripten>,$<BOOL:${POMDOG_ENABLE_EMSCRIPTEN_PTHREAD}>>:
        -pthread
    >
)

set_target_properties(zstd PROPERTIES
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2b"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK "YES"

    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"
    XCODE_ATTRIBUTE_SKIP_INSTALL "YES"
)
