cmake_minimum_required(VERSION 3.27)
project(catch2 CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(THIRDPARTY_DIR "../../thirdparty")

add_library(catch2_static STATIC
    ${THIRDPARTY_DIR}/catch2/extras/catch_amalgamated.cpp
    ${THIRDPARTY_DIR}/catch2/extras/catch_amalgamated.hpp
)

set_target_properties(catch2_static PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(catch2_static PRIVATE
    ${THIRDPARTY_DIR}/catch2/extras
)

target_compile_definitions(catch2_static PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        WIN32
        _LIB
        WIN32_LEAN_AND_MEAN
    >
)

target_compile_options(catch2_static PRIVATE
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
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
        /utf-8
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
        $<$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>:
            /fsanitize=address # NOTE: Enable address sanitizer
            /Zi # NOTE: Generate debugging information to support address sanitizer
        >

        # NOTE: https://github.com/catchorg/Catch2/issues/2174
        /Zc:hiddenFriend-
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        -pthread
    >
)

set_target_properties(catch2_static PROPERTIES
    LINKER_LANGUAGE CXX

    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"

    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c11"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2b"
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"

    # Symbols
    # XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES" # -fvisibility-inlines-hidden
    XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES" # -fvisibility=hidden
)
