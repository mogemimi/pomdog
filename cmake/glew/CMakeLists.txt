cmake_minimum_required(VERSION 3.27)
project(glew C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(THIRDPARTY_DIR "../../thirdparty")

add_library(glew_static STATIC
    ${THIRDPARTY_DIR}/vendor/glew/include/GL/glew.h
    ${THIRDPARTY_DIR}/vendor/glew/include/GL/glxew.h
    ${THIRDPARTY_DIR}/vendor/glew/include/GL/wglew.h
    ${THIRDPARTY_DIR}/vendor/glew/src/glew.c
)

target_include_directories(glew_static PRIVATE
    ${THIRDPARTY_DIR}/vendor/glew/include
)

target_compile_definitions(glew_static PRIVATE
    GLEW_STATIC
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        WIN32
        _LIB
        WIN32_LEAN_AND_MEAN
    >
)

target_compile_options(glew_static PRIVATE
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
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
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    >
)
