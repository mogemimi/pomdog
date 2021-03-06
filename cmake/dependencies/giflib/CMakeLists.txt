cmake_minimum_required(VERSION 3.16)
project(giflib C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(THIRDPARTY_DIR "../../../dependencies")

add_library(giflib STATIC
    ${THIRDPARTY_DIR}/giflib/dgif_lib.c
    ${THIRDPARTY_DIR}/giflib/egif_lib.c
    ${THIRDPARTY_DIR}/giflib/gif_err.c
    ${THIRDPARTY_DIR}/giflib/gif_font.c
    ${THIRDPARTY_DIR}/giflib/gif_hash.c
    ${THIRDPARTY_DIR}/giflib/gif_hash.h
    ${THIRDPARTY_DIR}/giflib/gif_lib_private.h
    ${THIRDPARTY_DIR}/giflib/gif_lib.h
    ${THIRDPARTY_DIR}/giflib/gifalloc.c
    ${THIRDPARTY_DIR}/giflib/openbsd-reallocarray.c
    ${THIRDPARTY_DIR}/giflib/quantize.c
)

target_include_directories(giflib PRIVATE
    ${THIRDPARTY_DIR}/giflib
    $<$<PLATFORM_ID:Windows>:
        ${THIRDPARTY_DIR}/vendor/giflib/windows
    >
)

target_compile_definitions(giflib PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        WIN32
        _WINDOWS
        _CRT_NONSTDC_NO_DEPRECATE
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_WARNINGS
    >
)

target_compile_options(giflib PRIVATE
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wno-implicit-function-declaration
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /nologo
        /W3
        /O2
        /Ob1
        /GF
        /Gy
        /GS-
        /wd"4013"
        /wd"4018"
        /wd"4028"
        /wd"4047"
        /wd"4244"
        /wd"4247"
        /wd"4267"
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    >
)

set_target_properties(giflib PROPERTIES
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2a"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_WEAK "YES"

    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.14"
    XCODE_ATTRIBUTE_SKIP_INSTALL "YES"
)
