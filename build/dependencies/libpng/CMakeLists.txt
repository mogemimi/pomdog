cmake_minimum_required(VERSION 3.10)
project(libpng C)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

set(POMDOG_DIR "../../..")
set(THIRDPARTY_DIR "../../../dependencies")

add_library(
  png STATIC
  ${THIRDPARTY_DIR}/vendor/libpng/pnglibconf.h
  ${THIRDPARTY_DIR}/libpng/png.c
  ${THIRDPARTY_DIR}/libpng/png.h
  ${THIRDPARTY_DIR}/libpng/pngconf.h
  ${THIRDPARTY_DIR}/libpng/pngdebug.h
  ${THIRDPARTY_DIR}/libpng/pngerror.c
  ${THIRDPARTY_DIR}/libpng/pngget.c
  ${THIRDPARTY_DIR}/libpng/pnginfo.h
  ${THIRDPARTY_DIR}/libpng/pngmem.c
  ${THIRDPARTY_DIR}/libpng/pngpread.c
  ${THIRDPARTY_DIR}/libpng/pngpriv.h
  ${THIRDPARTY_DIR}/libpng/pngread.c
  ${THIRDPARTY_DIR}/libpng/pngrio.c
  ${THIRDPARTY_DIR}/libpng/pngrtran.c
  ${THIRDPARTY_DIR}/libpng/pngrutil.c
  ${THIRDPARTY_DIR}/libpng/pngset.c
  ${THIRDPARTY_DIR}/libpng/pngstruct.h
  ${THIRDPARTY_DIR}/libpng/pngtrans.c
  ${THIRDPARTY_DIR}/libpng/pngwio.c
  ${THIRDPARTY_DIR}/libpng/pngwrite.c
  ${THIRDPARTY_DIR}/libpng/pngwtran.c
  ${THIRDPARTY_DIR}/libpng/pngwutil.c
)

target_include_directories(png PRIVATE
  ${THIRDPARTY_DIR}/libpng
  ${THIRDPARTY_DIR}/vendor/libpng
  ${THIRDPARTY_DIR}/zlib
)

target_compile_definitions(png PRIVATE
  $<$<CONFIG:DEBUG>:_DEBUG>
  $<$<CONFIG:RELEASE>:NDEBUG>
  $<$<PLATFORM_ID:Windows>:
    WIN32
    _WINDOWS
  >
)

target_compile_options(png PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    $<$<CONFIG:DEBUG>:-g;-O0>
    $<$<CONFIG:RELEASE>:-O3>
  >
  $<$<CXX_COMPILER_ID:MSVC>:
    /nologo
    /W4
    /Ox
    /Oi
    /Gm-
    /Gy
    /fp:except-
    /TC
    /GF
    /wd"4996"
    /wd"4127"
    /Zc:wchar_t-
    $<$<CONFIG:DEBUG>:/MTd>
    $<$<CONFIG:RELEASE>:/MT>
  >
)

set_target_properties(png PROPERTIES
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

add_subdirectory("${POMDOG_DIR}/build/dependencies/zlib" "${CMAKE_CURRENT_BINARY_DIR}/zlib_build")
target_link_libraries(png INTERFACE zlib)
