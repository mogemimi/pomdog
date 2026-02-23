cmake_minimum_required(VERSION 3.27)
project(fmt)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(THIRDPARTY_DIR "../../thirdparty")

add_library(fmt_static STATIC
    ${THIRDPARTY_DIR}/fmt/src/format.cc
)

set_target_properties(fmt_static PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(fmt_static PRIVATE
    ${THIRDPARTY_DIR}/fmt/include
)

target_compile_definitions(fmt_static PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        WIN32
        _WINDOWS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_NONSTDC_NO_DEPRECATE
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_WARNINGS

        # FIXME: Avoid compile errors on Visual Studio 2019
        FMT_USE_NONTYPE_TEMPLATE_ARGS=0
    >
)

target_compile_options(fmt_static PRIVATE
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wno-implicit-function-declaration
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>
    >
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>>:
        -fsanitize=address # NOTE: Enable address sanitizer
        -g # NOTE: Generate debugging information to support address sanitizer
    >
    $<$<CXX_COMPILER_ID:MSVC>:
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
        $<$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>:
            /fsanitize=address # NOTE: Enable address sanitizer
            /Zi # NOTE: Generate debugging information to support address sanitizer
        >
    >
)

set_target_properties(fmt_static PROPERTIES
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
