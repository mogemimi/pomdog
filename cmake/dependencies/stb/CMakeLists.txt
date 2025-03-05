cmake_minimum_required(VERSION 3.27)
project(stb C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(THIRDPARTY_DIR "../../../dependencies")

add_library(stb_static STATIC
    ${THIRDPARTY_DIR}/stb/stb_vorbis.c
    ${THIRDPARTY_DIR}/vendor/stb/include/stb/stb_vorbis.h
    ${THIRDPARTY_DIR}/stb/stb_truetype.h
    ${THIRDPARTY_DIR}/vendor/stb/stb_truetype.c
)

set_target_properties(stb_static PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(stb_static PRIVATE
    ${THIRDPARTY_DIR}/stb
    ${THIRDPARTY_DIR}/vendor/stb
)

target_compile_definitions(stb_static PRIVATE
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

target_compile_options(stb_static PRIVATE
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
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    >
)

set_target_properties(stb_static PROPERTIES
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
