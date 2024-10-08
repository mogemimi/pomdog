cmake_minimum_required(VERSION 3.27)
project(zlib C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(THIRDPARTY_DIR "../../../dependencies")

add_library(zlib STATIC
    ${THIRDPARTY_DIR}/zlib/adler32.c
    ${THIRDPARTY_DIR}/zlib/compress.c
    ${THIRDPARTY_DIR}/zlib/crc32.c
    ${THIRDPARTY_DIR}/zlib/crc32.h
    ${THIRDPARTY_DIR}/zlib/deflate.c
    ${THIRDPARTY_DIR}/zlib/deflate.h
    ${THIRDPARTY_DIR}/zlib/gzclose.c
    ${THIRDPARTY_DIR}/zlib/gzguts.h
    ${THIRDPARTY_DIR}/zlib/gzlib.c
    ${THIRDPARTY_DIR}/zlib/gzread.c
    ${THIRDPARTY_DIR}/zlib/gzwrite.c
    ${THIRDPARTY_DIR}/zlib/infback.c
    ${THIRDPARTY_DIR}/zlib/inffast.c
    ${THIRDPARTY_DIR}/zlib/inffast.h
    ${THIRDPARTY_DIR}/zlib/inffixed.h
    ${THIRDPARTY_DIR}/zlib/inflate.c
    ${THIRDPARTY_DIR}/zlib/inflate.h
    ${THIRDPARTY_DIR}/zlib/inftrees.c
    ${THIRDPARTY_DIR}/zlib/inftrees.h
    ${THIRDPARTY_DIR}/zlib/trees.c
    ${THIRDPARTY_DIR}/zlib/trees.h
    ${THIRDPARTY_DIR}/zlib/uncompr.c
    ${THIRDPARTY_DIR}/zlib/zconf.h
    ${THIRDPARTY_DIR}/zlib/zlib.h
    ${THIRDPARTY_DIR}/zlib/zutil.c
    ${THIRDPARTY_DIR}/zlib/zutil.h
)

target_include_directories(zlib PRIVATE
    ${THIRDPARTY_DIR}/zlib
)

target_compile_definitions(zlib PRIVATE
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

target_compile_options(zlib PRIVATE
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wno-implicit-function-declaration
        -Wno-shorten-64-to-32
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>
    >
    $<$<CXX_COMPILER_ID:Clang>:
        -Wno-deprecated-non-prototype
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /nologo
        /W3
        /O2
        /Ob1
        /GF
        /Gy
        /GS-
        /wd"4267"
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    >
)

if(UNIX)
    set_target_properties(zlib PROPERTIES OUTPUT_NAME z)
endif()

set_target_properties(zlib PROPERTIES
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
