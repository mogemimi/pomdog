cmake_minimum_required(VERSION 3.31)
project(pomdog CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

cmake_path(SET POMDOG_DIR     NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
cmake_path(SET POMDOG_SRC_DIR NORMALIZE "${POMDOG_DIR}/pomdog")

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(POMDOG_ENABLE_EMSCRIPTEN_PTHREAD 0 CACHE BOOL "Enable pthread in emscripten")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       true    CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       true    CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         true    CACHE BOOL   "Use X11")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(POMDOG_USE_DIRECT3D11  true    CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      false   CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     true    CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT true    CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
endif()
set(POMDOG_USE_EXPERIMENTAL true CACHE BOOL "Use Experimental")

option(POMDOG_USE_ADDRESS_SANITIZER "Instrument Pomdog and consumers with AddressSanitizer" OFF)
# NOTE: Cache defaults and toolchain checks run during configure, before genex evaluation.
if(POMDOG_USE_ADDRESS_SANITIZER)
    set(_pomdog_lto_default OFF)
else()
    set(_pomdog_lto_default ON)
endif()
option(POMDOG_USE_LTO "Enable explicit LTO flags in Release builds" ${_pomdog_lto_default})
if(POMDOG_USE_ADDRESS_SANITIZER AND POMDOG_USE_LTO)
    message(FATAL_ERROR "AddressSanitizer requires POMDOG_USE_LTO=OFF.")
endif()
if(POMDOG_USE_MEMORY_SANITIZER)
    message(FATAL_ERROR "MemorySanitizer is not supported by the bundled dependency builds. Use POMDOG_USE_ADDRESS_SANITIZER.")
endif()
if(POMDOG_USE_ADDRESS_SANITIZER AND NOT CMAKE_CXX_COMPILER_ID MATCHES "^(MSVC|Clang|AppleClang|GNU)$")
    message(FATAL_ERROR "AddressSanitizer is not configured for ${CMAKE_CXX_COMPILER_ID}.")
endif()

option(POMDOG_EXCLUDE_FROM_ALL "Build engine libraries only when explicitly requested or linked" OFF)

if(NOT CMAKE_SYSTEM_NAME MATCHES "^(Windows|Darwin|Linux|Emscripten)$")
    message(FATAL_ERROR "Pomdog does not support ${CMAKE_SYSTEM_NAME}.")
endif()
if((POMDOG_USE_DIRECT3D11 OR POMDOG_USE_VULKAN) AND NOT WIN32)
    message(FATAL_ERROR "The Direct3D11 and Vulkan backends are currently supported only on Windows.")
endif()
if(POMDOG_USE_METAL AND NOT APPLE)
    message(FATAL_ERROR "Metal requires macOS.")
endif()
if(WIN32 AND (NOT POMDOG_USE_XAUDIO2 OR POMDOG_USE_OPENAL))
    message(FATAL_ERROR "The Windows audio library requires XAudio2 and does not support OpenAL.")
elseif(NOT WIN32 AND (NOT POMDOG_USE_OPENAL OR POMDOG_USE_XAUDIO2))
    message(FATAL_ERROR "The audio library on ${CMAKE_SYSTEM_NAME} requires OpenAL, not XAudio2.")
endif()
if((WIN32 AND NOT POMDOG_USE_DIRECTINPUT) OR (APPLE AND NOT POMDOG_USE_IOKIT))
    message(FATAL_ERROR "The input library requires DirectInput on Windows and IOKit on macOS.")
endif()
if((POMDOG_USE_DIRECTINPUT AND NOT WIN32) OR (POMDOG_USE_IOKIT AND NOT APPLE))
    message(FATAL_ERROR "The selected input backend is unavailable on ${CMAKE_SYSTEM_NAME}.")
endif()
if(WIN32 AND NOT (POMDOG_USE_DIRECT3D11 OR POMDOG_USE_GL4 OR POMDOG_USE_VULKAN))
    message(FATAL_ERROR "application_gpu needs a Windows GPU backend.")
elseif(APPLE AND (NOT POMDOG_USE_GL4 OR NOT POMDOG_USE_METAL OR NOT POMDOG_USE_IOKIT))
    message(FATAL_ERROR "The current Cocoa host requires GL4, Metal, and IOKit. Host backend separation is pending.")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND (NOT POMDOG_USE_GL4 OR NOT POMDOG_USE_X11))
    message(FATAL_ERROR "The current Linux host requires GL4 and X11.")
elseif(EMSCRIPTEN AND NOT POMDOG_USE_GL4)
    message(FATAL_ERROR "The Browser host requires GL4 (WebGL2).")
endif()

include(pomdog_base.cmake)
include(pomdog_math.cmake)
include(pomdog_random.cmake)
include(pomdog_filesystem.cmake)
include(pomdog_vfs.cmake)
include(pomdog_input.cmake)
include(pomdog_gpu.cmake)
include(pomdog_audio.cmake)
include(pomdog_network.cmake)
include(pomdog_application.cmake)
include(pomdog_application_gpu.cmake)
include(pomdog_content_utility.cmake)
include(pomdog_content_gpu.cmake)
include(pomdog_content_audio.cmake)
include(pomdog_content_input.cmake)
include(pomdog_graphics.cmake)
include(pomdog_ecs.cmake)
include(pomdog_console.cmake)
include(pomdog_experimental.cmake)

# NOTE: Compatibility target for applications that use the complete engine.
add_library(pomdog_static INTERFACE)

add_library(pomdog_lto_settings INTERFACE)
target_compile_options(pomdog_lto_settings INTERFACE
    $<$<AND:$<CONFIG:Release>,$<BOOL:${POMDOG_USE_LTO}>>:
        $<$<CXX_COMPILER_ID:MSVC>:
            /GL
        >
        $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
            -flto
        >
    >
)
target_link_options(pomdog_lto_settings INTERFACE
    $<$<AND:$<CONFIG:Release>,$<BOOL:${POMDOG_USE_LTO}>>:
        $<$<CXX_COMPILER_ID:MSVC>:
            /LTCG
            /INCREMENTAL:NO
        >
        $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
            -flto
        >
    >
)

add_library(pomdog_sanitizer_settings INTERFACE)
target_compile_options(pomdog_sanitizer_settings INTERFACE
    $<$<AND:$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>,$<CXX_COMPILER_ID:MSVC>>:
        /fsanitize=address
        /Zi
    >
    $<$<AND:$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>,$<CXX_COMPILER_ID:Clang,AppleClang,GNU>>:
        -fsanitize=address
        -fno-omit-frame-pointer
        -g
    >
)
target_link_options(pomdog_sanitizer_settings INTERFACE
    $<$<AND:$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>,$<CXX_COMPILER_ID:MSVC>>:
        /INCREMENTAL:NO
        /DEBUG
    >
    $<$<AND:$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>,$<CXX_COMPILER_ID:Clang,AppleClang,GNU>>:
        -fsanitize=undefined,address
    >
)

add_library(pomdog_build_settings INTERFACE)
target_link_libraries(pomdog_build_settings INTERFACE
    pomdog_sanitizer_settings
    pomdog_lto_settings
)

set(POMDOG_COMPILE_TARGETS
    pomdog_base
    pomdog_math
    pomdog_random
    pomdog_filesystem
    pomdog_vfs
    pomdog_input
    pomdog_gpu
    pomdog_audio
    pomdog_network
    pomdog_application
    pomdog_application_gpu
    pomdog_content_utility
    pomdog_content_gpu
    pomdog_content_audio
    pomdog_content_input
    pomdog_graphics
    pomdog_ecs
    pomdog_console
    pomdog_experimental
)

set_target_properties(${POMDOG_COMPILE_TARGETS} PROPERTIES
    EXCLUDE_FROM_ALL "$<BOOL:${POMDOG_EXCLUDE_FROM_ALL}>"
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF

    # NOTE: Sources use headers, not C++ named modules (CMP0155).
    CXX_SCAN_FOR_MODULES OFF
)

add_subdirectory(${POMDOG_DIR}/cmake/fmt "${CMAKE_CURRENT_BINARY_DIR}/fmt_build" EXCLUDE_FROM_ALL)
add_subdirectory(${POMDOG_DIR}/cmake/zlib "${CMAKE_CURRENT_BINARY_DIR}/zlib_build" EXCLUDE_FROM_ALL)
add_subdirectory(${POMDOG_DIR}/cmake/libpng "${CMAKE_CURRENT_BINARY_DIR}/libpng_build" EXCLUDE_FROM_ALL)
add_subdirectory(${POMDOG_DIR}/cmake/stb "${CMAKE_CURRENT_BINARY_DIR}/stb_build" EXCLUDE_FROM_ALL)
target_link_libraries(fmt_static PRIVATE pomdog_sanitizer_settings pomdog_lto_settings)
target_link_libraries(zlib PRIVATE pomdog_sanitizer_settings pomdog_lto_settings)
target_link_libraries(png PRIVATE pomdog_sanitizer_settings pomdog_lto_settings)
target_link_libraries(stb_static PRIVATE pomdog_sanitizer_settings pomdog_lto_settings)
if(NOT (CMAKE_SYSTEM_NAME STREQUAL "Emscripten"))
    add_subdirectory(${POMDOG_DIR}/cmake/mbedtls "${CMAKE_CURRENT_BINARY_DIR}/mbedtls_build" EXCLUDE_FROM_ALL)
    target_link_libraries(mbedtls PRIVATE pomdog_sanitizer_settings pomdog_lto_settings)
endif()
if(POMDOG_USE_GL4 AND ((CMAKE_SYSTEM_NAME STREQUAL "Windows") OR (CMAKE_SYSTEM_NAME STREQUAL "Linux")))
    add_subdirectory(${POMDOG_DIR}/cmake/glew "${CMAKE_CURRENT_BINARY_DIR}/glew_build" EXCLUDE_FROM_ALL)
    target_link_libraries(glew_static PRIVATE pomdog_sanitizer_settings pomdog_lto_settings)
endif()
if(POMDOG_USE_VULKAN)
    find_package(Vulkan REQUIRED)
endif()

# NOTE: Run bootstrap.sh or build_tools.sh before building schema consumers.
# CMake consumes these headers; it neither runs flatc nor updates stale output.
target_include_directories(pomdog_build_settings INTERFACE
    ${POMDOG_DIR}
    ${POMDOG_DIR}/build/schemas-cpp
    ${POMDOG_DIR}/thirdparty
    ${POMDOG_DIR}/thirdparty/flatbuffers/include
)

target_link_libraries(pomdog_static INTERFACE
    pomdog::application
    pomdog::application_gpu
    pomdog::audio
    pomdog::base
    pomdog::console
    pomdog::content_audio
    pomdog::content_gpu
    pomdog::content_input
    pomdog::content_utility
    pomdog::ecs
    pomdog::experimental
    pomdog::filesystem
    pomdog::gpu
    pomdog::graphics
    pomdog::input
    pomdog::math
    pomdog::network
    pomdog::random
    pomdog::vfs
)

target_compile_definitions(pomdog_build_settings INTERFACE
    $<$<CONFIG:Debug>:POMDOG_DEBUG_BUILD=1>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        _WIN32_WINNT=0x0A00 # NOTE: Windows 10 or later
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
)

target_compile_options(pomdog_build_settings INTERFACE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wall
        -Werror
        -Wnon-virtual-dtor

        $<$<PLATFORM_ID:Linux,Windows,Darwin>:
            -mavx # NOTE: Enable AVX for GCC and Clang
        >

        $<$<PLATFORM_ID:Linux,Windows,Darwin>:
            # Linux, Windows and macOS
            $<$<CONFIG:Debug>:-g;-O0>
            $<$<CONFIG:Release>:-O3>
        >
        $<$<PLATFORM_ID:Emscripten>:
            # Emscripten
            $<$<CONFIG:Debug>:-g3;-O0>
            $<$<CONFIG:Release>:-O3>
        >

        # NOTE: excluding clang-cl
        $<$<NOT:$<BOOL:MSVC>>:
            -pedantic
            -pedantic-errors
            -fvisibility=hidden
        >
    >
    # NOTE: Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++98-compat-bind-to-temporary-copy
        -Wno-c++98-compat-local-type-template-args
        -Wno-padded

        # FIXME: Remove the following `-Wno-...` flags.
        -Wno-float-equal
        -Wno-shadow-uncaptured-local
        -Wno-sign-conversion
        -Wno-switch-enum
        -Wno-undefined-func-template
        -Wno-implicit-int-float-conversion

        # FIXME: Conflict between C++20 aggregate initializations and C99 desinated initializers.
        -Wno-c99-extensions

        # FIXME: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#vague-itemplate
        # FIXME: https://bugs.llvm.org/show_bug.cgi?id=18733
        -Wno-weak-template-vtables

        # FIXME: https://github.com/llvm/llvm-project/issues/81354
        -Wno-switch-default

        $<$<PLATFORM_ID:Linux>:
            # FIXME: Remove the following `-Wno` flags for Linux.
            -Wno-unsafe-buffer-usage
        >
    >
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,21>>:
        # NOTE: Disable thread safety analysis for Linux with Clang >= 21, because it causes false positives.
        -Wno-thread-safety-negative

        # FIXME: Disable NRVO warning, because it causes false positives in some cases.
        -Wno-nrvo
    >
    $<$<CXX_COMPILER_ID:AppleClang>:
        # FIXME: Remove the following `-Wno` flags.
        -Wno-direct-ivar-access
        -Wno-objc-messaging-id

        # NOTE: Xcode adds include search paths for directories (e.g. DerivedSources)
        # that may not exist on the first build. Keep the warning but prevent -Werror
        # from promoting it to an error.
        -Wno-error=missing-include-dirs
    >
    # NOTE: GCC
    $<$<CXX_COMPILER_ID:GNU>:
        -Wextra
        -Wcast-align
        -Wcast-qual
        # -Wconversion
        -Wctor-dtor-privacy
        -Wdelete-non-virtual-dtor
        -Wdisabled-optimization
        -Wdouble-promotion
        -Wendif-labels
        -Wformat-nonliteral
        -Wformat-security
        -Wformat-signedness
        -Winit-self
        # -Winline
        -Wlogical-op
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wredundant-decls
        -Wreorder
        -Wshadow
        -Wstrict-null-sentinel
        # -Wsuggest-final-methods
        -Wundef
        -Wunsafe-loop-optimizations
        # -Wuseless-cast
        -Wvariadic-macros
        -Wwrite-strings
        -Wzero-as-null-pointer-constant
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        # NOTE: See https://docs.microsoft.com/en-us/cpp/preprocessor/compiler-warnings-that-are-off-by-default
        /Wall
        /WX
        /wd4061 # NOTE: enumerator 'E::X' in switch of enum 'E' is not explicitly handled by a case label
        /wd4365 # NOTE: signed/unsigned mismatch
        /wd4514 # NOTE: unreferenced inline function has been removed
        /wd4625 # NOTE: copy constructor was implicitly defined as deleted
        /wd4626 # NOTE: assignment operator was implicitly defined as deleted
        /wd4710 # NOTE: function not inlined
        /wd4711 # NOTE: function 'F' selected for automatic inline expansion
        /wd4820 # NOTE: 'N' bytes padding added after data member
        /wd5026 # NOTE: move constructor was implicitly defined as deleted
        /wd5027 # NOTE: move assignment operator was implicitly defined as deleted
        /wd5039 # NOTE: Undefined behavior may occur if this function throws an exception.
        /wd5045 # NOTE: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd5219 # NOTE: implicit conversion from 'int' to 'float', possible loss of data
        /std:c++latest
        /permissive-
        /utf-8
        /MP # NOTE: Build with multiple processes for faster builds
        $<$<CONFIG:Debug>:/Od;/MTd>
        $<$<CONFIG:Release>:/O2;/Ob2;/MT>
        /arch:AVX # NOTE: Enable AVX for MSVC
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        $<$<BOOL:${POMDOG_ENABLE_EMSCRIPTEN_PTHREAD}>:
            # NOTE: pthreads can only be used in browsers that have SharedArrayBuffer enabled.
            -pthread
        >

        # FIXME: Please fix the following warning and enable the warning option.
        -Wno-unsafe-buffer-usage

        # NOTE: Disable thread safety analysis for Emscripten, because it causes false positives.
        -Wno-thread-safety-negative

        # FIXME: Disable NRVO warning, because it causes false positives in some cases.
        -Wno-nrvo

        # NOTE: Disable the experimental lifetime-safety diagnostics (Clang >= 21).
        # They fire false positives on standard container patterns such as `vector::push_back`.
        $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,21>:
            -Wno-lifetime-safety-cross-tu-suggestions
            -Wno-lifetime-safety-intra-tu-suggestions
        >
        $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,23>:
            -Wno-lifetime-safety-invalidation
        >
    >
)

# NOTE: For dynamic library
# NOTE: see https://cmake.org/cmake/help/v3.9/prop_tgt/FRAMEWORK.html
# set_target_properties(pomdog_static PROPERTIES
#   XCODE_ATTRIBUTE_PRODUCT_NAME "pomdog"
#   XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "net.enginetrouble.pomdog"
#   XCODE_ATTRIBUTE_INFOPLIST_FILE "Info.plist"
#   XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_LIBRARY_DIR)/Frameworks"
#   XCODE_ATTRIBUTE_DYLIB_INSTALL_NAME_BASE "@rpath"
#   XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$(inherited);@executable_path/../Frameworks;@loader_path/Frameworks"
# )

set_target_properties(${POMDOG_COMPILE_TARGETS} PROPERTIES
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c11"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2b"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"

    XCODE_ATTRIBUTE_SKIP_INSTALL "YES"

    # Warnings (Clang)
    XCODE_ATTRIBUTE_CLANG_WARN_ASSIGN_ENUM "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_BOOL_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_COMMA "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_CONSTANT_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_DOCUMENTATION_COMMENTS "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_EMPTY_BODY "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_ENUM_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_INFINITE_RECURSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_INT_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_NON_LITERAL_NULL_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_RANGE_LOOP_ANALYSIS "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_STRICT_PROTOTYPES "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_SUSPICIOUS_MOVE "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_UNREACHABLE_CODE "YES"

    # Warnings (GCC and Clang)
    XCODE_ATTRIBUTE_GCC_TREAT_INCOMPATIBLE_POINTER_TYPE_WARNINGS_AS_ERRORS "YES"
    XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "YES"
    XCODE_ATTRIBUTE_GCC_WARN_ABOUT_DEPRECATED_FUNCTIONS "YES"
    XCODE_ATTRIBUTE_GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS "YES"
    XCODE_ATTRIBUTE_GCC_WARN_ABOUT_MISSING_NEWLINE "YES"
    XCODE_ATTRIBUTE_GCC_WARN_ABOUT_RETURN_TYPE "YES_ERROR"
    XCODE_ATTRIBUTE_GCC_WARN_CHECK_SWITCH_STATEMENTS "YES"
    XCODE_ATTRIBUTE_GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS "YES"
    XCODE_ATTRIBUTE_GCC_WARN_MISSING_PARENTHESES "YES"
    XCODE_ATTRIBUTE_GCC_WARN_NON_VIRTUAL_DESTRUCTOR "YES"
    XCODE_ATTRIBUTE_GCC_WARN_SHADOW "YES"
    XCODE_ATTRIBUTE_GCC_WARN_SIGN_COMPARE "YES"
    XCODE_ATTRIBUTE_GCC_WARN_TYPECHECK_CALLS_TO_PRINTF "YES"
    XCODE_ATTRIBUTE_GCC_WARN_UNINITIALIZED_AUTOS "YES_AGGRESSIVE"
    XCODE_ATTRIBUTE_GCC_WARN_UNKNOWN_PRAGMAS "YES"
    XCODE_ATTRIBUTE_GCC_WARN_UNUSED_FUNCTION "YES"
    XCODE_ATTRIBUTE_GCC_WARN_UNUSED_LABEL "YES"
    XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VALUE "YES"
    XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VARIABLE "YES"

    # Warnings - Objective-C
    XCODE_ATTRIBUTE_CLANG_WARN_DIRECT_OBJC_ISA_USAGE "YES_ERROR"
    XCODE_ATTRIBUTE_CLANG_WARN__DUPLICATE_METHOD_MATCH "YES"
    XCODE_ATTRIBUTE_GCC_WARN_ALLOW_INCOMPLETE_PROTOCOL "YES"
    XCODE_ATTRIBUTE_GCC_WARN_UNDECLARED_SELECTOR "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_OBJC_LITERAL_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_OBJC_ROOT_CLASS "YES_ERROR"

    # Warning Policies
    XCODE_ATTRIBUTE_GCC_TREAT_WARNINGS_AS_ERRORS "YES"
    XCODE_ATTRIBUTE_WARNING_CFLAGS "-Wall"

    # Metal Compiler
    XCODE_ATTRIBUTE_MTL_ENABLE_DEBUG_INFO[variant=Debug] "INCLUDE_SOURCE"
    XCODE_ATTRIBUTE_MTL_ENABLE_DEBUG_INFO[variant=Release] "NO"
    XCODE_ATTRIBUTE_MTL_FAST_MATH "YES"

    # Static Analyzer
    XCODE_ATTRIBUTE_CLANG_ANALYZER_LOCALIZABILITY_NONLOCALIZED "YES"

    # Symbols
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES" # -fvisibility-inlines-hidden
    XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES" # -fvisibility=hidden
)
