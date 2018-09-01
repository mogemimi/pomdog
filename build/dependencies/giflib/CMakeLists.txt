cmake_minimum_required(VERSION 3.10)
project(giflib C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(THIRDPARTY_DIR "../../../dependencies")

add_library(giflib STATIC
  ${THIRDPARTY_DIR}/vendor/giflib/lib/dgif_lib.c
  ${THIRDPARTY_DIR}/vendor/giflib/lib/egif_lib.c
  ${THIRDPARTY_DIR}/vendor/giflib/lib/gif_err.c
  ${THIRDPARTY_DIR}/vendor/giflib/lib/gif_font.c
  ${THIRDPARTY_DIR}/vendor/giflib/lib/gif_hash.c
  ${THIRDPARTY_DIR}/vendor/giflib/lib/gif_hash.h
  ${THIRDPARTY_DIR}/vendor/giflib/lib/gif_lib_private.h
  ${THIRDPARTY_DIR}/vendor/giflib/lib/gif_lib.h
  ${THIRDPARTY_DIR}/vendor/giflib/lib/gifalloc.c
  ${THIRDPARTY_DIR}/vendor/giflib/lib/openbsd-reallocarray.c
  ${THIRDPARTY_DIR}/vendor/giflib/lib/quantize.c
)

target_include_directories(giflib PRIVATE
  ${THIRDPARTY_DIR}/vendor/giflib/lib
  $<$<PLATFORM_ID:Windows>:
    ${THIRDPARTY_DIR}/vendor/giflib/windows
  >
)

target_compile_definitions(giflib PRIVATE
  $<$<CONFIG:DEBUG>:_DEBUG>
  $<$<CONFIG:RELEASE>:NDEBUG>
  $<$<PLATFORM_ID:Windows>:
    WIN32
    _WINDOWS
    _CRT_NONSTDC_NO_DEPRECATE
    _CRT_SECURE_NO_DEPRECATE
    _CRT_NONSTDC_NO_WARNINGS
  >
)

target_compile_options(giflib PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wno-implicit-function-declaration
    $<$<CONFIG:DEBUG>:-g;-O0>
    $<$<CONFIG:RELEASE>:-O3>
  >
  $<$<CXX_COMPILER_ID:MSVC>:
    /nologo
    /W3
    /O2
    /Ob1
    /GF
    /Gy
    /GS-
    /wd"4018"
    /wd"4028"
    /wd"4244"
    $<$<CONFIG:DEBUG>:/MTd>
    $<$<CONFIG:RELEASE>:/MT>
  >
)

set_target_properties(giflib PROPERTIES
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
  XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
  XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

  XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
  XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17"
  XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"

  XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.11"
  XCODE_ATTRIBUTE_SKIP_INSTALL "YES"
)
