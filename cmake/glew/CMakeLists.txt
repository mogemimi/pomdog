cmake_minimum_required(VERSION 3.31)
project(glew C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

cmake_path(SET THIRDPARTY_DIR NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty")

add_library(glew_static STATIC
    ${THIRDPARTY_DIR}/vendor/glew/include/GL/glew.h
    ${THIRDPARTY_DIR}/vendor/glew/include/GL/glxew.h
    ${THIRDPARTY_DIR}/vendor/glew/include/GL/wglew.h
    ${THIRDPARTY_DIR}/vendor/glew/src/glew.c
)

target_include_directories(glew_static
    PUBLIC
        ${THIRDPARTY_DIR}/vendor/glew/include
)

target_compile_definitions(glew_static PUBLIC
    GLEW_STATIC
)
target_compile_definitions(glew_static PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        WIN32
        _LIB
        WIN32_LEAN_AND_MEAN
    >
)

target_compile_options(glew_static PRIVATE
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
        /utf-8
        /MP # NOTE: Build with multiple processes for faster builds
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    >
)
