cmake_minimum_required(VERSION 3.31)
project(pomdog_app_settings CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_ENABLE_EMSCRIPTEN_PTHREAD 0 CACHE BOOL "Enable pthread in emscripten")

add_library(pomdog_app_settings INTERFACE)

target_link_libraries(pomdog_app_settings INTERFACE
    pomdog_app_settings_base
    pomdog_app_settings_optimization
    pomdog_app_settings_warnings
)

add_library(pomdog_app_settings_base INTERFACE)

target_link_libraries(pomdog_app_settings_base INTERFACE
    pomdog_sanitizer_settings
)

target_compile_definitions(pomdog_app_settings_base INTERFACE
    $<$<CONFIG:Debug>:POMDOG_DEBUG_BUILD=1>
    $<$<CONFIG:Release>:NDEBUG>

    # On Windows
    $<$<PLATFORM_ID:Windows>:
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
)

target_compile_options(pomdog_app_settings_base INTERFACE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        $<$<PLATFORM_ID:Linux,Windows,Darwin>:
            # Linux, Windows and macOS
            $<$<CONFIG:Debug>:-g>
        >
        $<$<PLATFORM_ID:Emscripten>:
            # Emscripten
            $<$<CONFIG:Debug>:-g3>
        >
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        /std:c++latest
        /utf-8
        /MP # NOTE: Build with multiple processes for faster builds
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        $<$<BOOL:${POMDOG_ENABLE_EMSCRIPTEN_PTHREAD}>:
            # NOTE: pthreads can only be used in browsers that have SharedArrayBuffer enabled.
            -pthread
        >
    >
)

target_link_options(pomdog_app_settings_base INTERFACE
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<AND:$<CONFIG:Debug>,$<NOT:$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>>>:
            /INCREMENTAL
        >
        $<$<CONFIG:Debug>:
            /DEBUG
        >
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        "SHELL:-s EXPORTED_RUNTIME_METHODS=stringToNewUTF8"

        # NOTE: Enable WASM for Emscripten
        "SHELL:-s WASM=1"

        $<$<BOOL:${POMDOG_ENABLE_EMSCRIPTEN_PTHREAD}>:
            # NOTE: pthreads can only be used in browsers that have SharedArrayBuffer enabled.
            "SHELL:-s USE_PTHREADS=1"
        >

        # NOTE: For using large memory.
        "SHELL:-s ALLOW_MEMORY_GROWTH"
    >
)

add_library(pomdog_app_settings_optimization INTERFACE)

target_link_libraries(pomdog_app_settings_optimization INTERFACE
    pomdog_lto_settings
)

target_compile_options(pomdog_app_settings_optimization INTERFACE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        $<$<PLATFORM_ID:Linux,Windows,Darwin>:
            -mavx # NOTE: Enable AVX for GCC and Clang
        >
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-O3>
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Release>:/O2;/Ob2>
        /arch:AVX # NOTE: Enable AVX for MSVC
    >
)

target_link_options(pomdog_app_settings_optimization INTERFACE
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Release>:
            /INCREMENTAL:NO
            /OPT:REF
        >
    >
)

add_library(pomdog_app_settings_warnings INTERFACE)

target_compile_options(pomdog_app_settings_warnings INTERFACE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wall
    >
    # NOTE: Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Wbool-conversion
        -Wcast-qual
        -Wcomma
        -Wconditional-uninitialized
        -Wconstant-conversion
        -Wdeprecated-copy
        -Wdeprecated-declarations
        -Wduplicate-enum
        -Wempty-body
        -Wenum-conversion
        -Wextra-semi
        -Wheader-hygiene
        -Wimplicit-fallthrough
        -Wint-conversion
        -Wmove
        -Wnewline-eof
        -Wnon-virtual-dtor
        -Wnull-pointer-arithmetic
        -Wshadow
        -Wshadow-field
        -Wshorten-64-to-32
        -Wsign-compare
        -Wstring-concatenation
        -Wundef
        -Wunguarded-availability
        -Wunreachable-code
        -Wvla
    >
    # NOTE: AppleClang (Objective-C)
    $<$<CXX_COMPILER_ID:AppleClang>:
        -Wdeprecated-implementations
        -Wdeprecated-objc-isa-usage
        -Wduplicate-method-match
        -Wimplicit-retain-self
        -Wobjc-literal-conversion
        -Wobjc-root-class
        -Wprotocol
        -Wundeclared-selector
    >
    # NOTE: GCC
    $<$<CXX_COMPILER_ID:GNU>:
        -pedantic
        -Wimplicit-fallthrough=3
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /w44062 # NOTE: enumerator in switch of enum is not handled (-Wswitch)
        /w44263 # NOTE: member function does not override any base class virtual member function (-Woverloaded-virtual)
        /w44264 # NOTE: no override available for virtual member function from base; function is hidden (-Woverloaded-virtual)
        /w44265 # NOTE: class has virtual functions, but its non-trivial destructor is not virtual (-Wnon-virtual-dtor)
        /w44266 # NOTE: no override available for virtual member function from base; function is hidden (-Woverloaded-virtual)
        /w44287 # NOTE: unsigned/negative constant mismatch (-Wsign-compare)
        /w44388 # NOTE: signed/unsigned mismatch (-Wsign-compare)
        /w44545 # NOTE: expression before comma evaluates to a function which is missing an argument list (-Wcomma)
        /w44546 # NOTE: function call before comma missing argument list (-Wcomma)
        /w44547 # NOTE: operator before comma has no effect; expected operator with side-effect (-Wcomma)
        /w44548 # NOTE: expression before comma has no effect; expected expression with side-effect (-Wcomma)
        /w44549 # NOTE: operator before comma has no effect; did you intend another operator? (-Wcomma)
        /w44555 # NOTE: expression has no effect; expected expression with side-effect (-Wunused-value)
        /w44777 # NOTE: format string requires an argument of a different type (-Wformat)
        /w44946 # NOTE: reinterpret_cast used between related classes (-Wreinterpret-base-class)
        /w45038 # NOTE: data member will be initialized after another data member (-Wreorder)
        /w45204 # NOTE: class has virtual functions, but its trivial destructor is not virtual (-Wnon-virtual-dtor)
        /w45233 # NOTE: explicit lambda capture is not used (-Wunused-lambda-capture)
        /w45245 # NOTE: unreferenced function with internal linkage has been removed (-Wunused-function)
        /w45258 # NOTE: explicit capture is not required for this use (-Wunused-lambda-capture)
        /w45262 # NOTE: implicit fall-through occurs here (-Wimplicit-fallthrough)
        /w45263 # NOTE: calling std::move on a temporary object prevents copy elision (-Wpessimizing-move)
        /w45264 # NOTE: 'const' variable is not used (-Wunused-const-variable)
        /w45267 # NOTE: implicit copy constructor or assignment operator is deprecated (-Wdeprecated-copy)
    >
)
