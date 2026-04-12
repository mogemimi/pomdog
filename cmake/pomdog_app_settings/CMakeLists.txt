cmake_minimum_required(VERSION 3.27)
project(pomdog_app_settings CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_ENABLE_EMSCRIPTEN_PTHREAD 0 CACHE BOOL "Enable pthread in emscripten")

add_library(pomdog_app_settings INTERFACE)

target_compile_definitions(pomdog_app_settings INTERFACE
    $<$<CONFIG:Debug>:_DEBUG;POMDOG_DEBUG_BUILD=1>
    $<$<CONFIG:Release>:NDEBUG>

    # On Windows
    $<$<PLATFORM_ID:Windows>:
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
)

target_compile_options(pomdog_app_settings INTERFACE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Werror

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
    >
    # NOTE: Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Wcomma
        -Wextra-semi
        -Wimplicit-fallthrough
        -Wmost
        -Wmove
        -Wnewline-eof
        -Wshorten-64-to-32
    >
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>>:
        -fsanitize=address # NOTE: Enable address sanitizer
        -g # NOTE: Generate debugging information to support address sanitizer
    >
    # NOTE: GCC
    $<$<CXX_COMPILER_ID:GNU>:
        -pedantic
        -Wimplicit-fallthrough=3
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /std:c++latest
        /utf-8
        /MP # NOTE: Build with multiple processes for faster builds
        $<$<CONFIG:Debug>:/Od;/MTd>
        $<$<CONFIG:Release>:/O2;/Ob2;/MT>
        $<$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>:
            /fsanitize=address # NOTE: Enable address sanitizer
            /Zi # NOTE: Generate debugging information to support address sanitizer
        >
        /arch:AVX # NOTE: Enable AVX for MSVC
        $<$<CONFIG:Release>:/GL> # NOTE: For LTO build
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        $<$<BOOL:${POMDOG_ENABLE_EMSCRIPTEN_PTHREAD}>:
            # NOTE: pthreads can only be used in browsers that have SharedArrayBuffer enabled.
            -pthread
        >
    >
)

target_link_options(pomdog_app_settings INTERFACE
    # NOTE: Apple macOS
    $<$<PLATFORM_ID:Darwin>:
        "SHELL: -framework AudioToolBox"
        "SHELL: -framework Cocoa"
        "SHELL: -framework IOKit"
        "SHELL: -framework Metal"
        "SHELL: -framework MetalKit"
        "SHELL: -framework OpenAL"
        "SHELL: -framework OpenGL"
        "SHELL: -framework QuartzCore"
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<AND:$<CONFIG:Debug>,$<NOT:$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>>>:
            /INCREMENTAL
        >
        $<$<OR:$<CONFIG:Debug>,$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>>:
            /DEBUG
        >
        $<$<CONFIG:Release>:
            /INCREMENTAL:NO
            /OPT:REF
            /LTCG # NOTE: For LTO build
        >
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        "SHELL:-s OFFSCREEN_FRAMEBUFFER=1"
        "SHELL:-s USE_WEBGL2=1"
        "SHELL:-s MIN_WEBGL_VERSION=2"
        "SHELL:-s MAX_WEBGL_VERSION=2"
        "SHELL:-s FULL_ES3=1"
        "SHELL:-s EXPORTED_RUNTIME_METHODS=stringToNewUTF8"

        # NOTE: Enable WASM for Emscripten
        "SHELL:-s WASM=1"

        # NOTE: Enable file system support for Emscripten
        "SHELL:-lidbfs.js"

        $<$<BOOL:${POMDOG_ENABLE_EMSCRIPTEN_PTHREAD}>:
            # NOTE: pthreads can only be used in browsers that have SharedArrayBuffer enabled.
            "SHELL:-s USE_PTHREADS=1"
        >

        # NOTE: For using large memory.
        "SHELL:-s ALLOW_MEMORY_GROWTH"
    >
    # NOTE: Clang LTO
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:Release>>:
        -flto
    >
    # NOTE: clang sanitizer
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>>:
        -fsanitize=undefined,address
    >
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<BOOL:${POMDOG_USE_MEMORY_SANITIZER}>>:
        -fsanitize=memory
    >
)
