cmake_minimum_required(VERSION 3.27)
project(pomdog_tests CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_DIR "..")
set(POMDOG_TEST_DIR "${POMDOG_DIR}/tests")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(chrono                 REGULAR_EXPRESSION tests/chrono/*)
source_group(ecs                    REGULAR_EXPRESSION tests/ecs/*)
source_group(content                REGULAR_EXPRESSION tests/content/*)
source_group(experimental\\async    REGULAR_EXPRESSION tests/experimental/async/*)
source_group(experimental\\random   REGULAR_EXPRESSION tests/experimental/random/*)
source_group(filesystem             REGULAR_EXPRESSION tests/filesystem/*)
source_group(graphics               REGULAR_EXPRESSION tests/graphics/*)
source_group(input                  REGULAR_EXPRESSION tests/input/*)
source_group(logging                REGULAR_EXPRESSION tests/logging/*)
source_group(math                   REGULAR_EXPRESSION tests/math/*)
source_group(memory                 REGULAR_EXPRESSION tests/memory/*)
source_group(network                REGULAR_EXPRESSION tests/network/*)
source_group(signals                REGULAR_EXPRESSION tests/signals/*)
source_group(testing                REGULAR_EXPRESSION tests/testing/*)
source_group(utility                REGULAR_EXPRESSION tests/utility/*)

add_executable(pomdog_tests)

target_sources(pomdog_tests PRIVATE
    ${POMDOG_TEST_DIR}/chrono/game_clock_test.cpp
    ${POMDOG_TEST_DIR}/chrono/time_source_test.cpp
    ${POMDOG_TEST_DIR}/chrono/timer_test.cpp
    ${POMDOG_TEST_DIR}/ecs/entity_bin_test.cpp
    ${POMDOG_TEST_DIR}/ecs/entity_lru_queue_test.cpp
    ${POMDOG_TEST_DIR}/ecs/entity_pool_test.cpp
    ${POMDOG_TEST_DIR}/ecs/entity_test.cpp
    ${POMDOG_TEST_DIR}/ecs/tagged_entity_test.cpp
    ${POMDOG_TEST_DIR}/experimental/random/xoroshiro128_star_star_test.cpp
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # TODO: Need to implement for Emscripten.
        ${POMDOG_TEST_DIR}/filesystem/file_system_test.cpp
    >
    ${POMDOG_TEST_DIR}/graphics/input_layout_helper_test.cpp
    ${POMDOG_TEST_DIR}/input/gamepad_uuid_test.cpp
    ${POMDOG_TEST_DIR}/input/keyboard_state_test.cpp
    ${POMDOG_TEST_DIR}/input/keys_test.cpp
    ${POMDOG_TEST_DIR}/input/mouse_state_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_box_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_box2d_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_circle_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_frustum_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_sphere_test.cpp
    ${POMDOG_TEST_DIR}/math/color_test.cpp
    ${POMDOG_TEST_DIR}/math/degree_test.cpp
    ${POMDOG_TEST_DIR}/math/math_constants_test.cpp
    ${POMDOG_TEST_DIR}/math/math_functions_test.cpp
    ${POMDOG_TEST_DIR}/math/point2d_test.cpp
    ${POMDOG_TEST_DIR}/math/point3d_test.cpp
    ${POMDOG_TEST_DIR}/math/quaternion_test.cpp
    ${POMDOG_TEST_DIR}/math/radian_test.cpp
    ${POMDOG_TEST_DIR}/math/rectangle_test.cpp
    ${POMDOG_TEST_DIR}/memory/aligned_alloc_test.cpp
    ${POMDOG_TEST_DIR}/memory/alignment_test.cpp
    ${POMDOG_TEST_DIR}/memory/linear_allocator_test.cpp
    ${POMDOG_TEST_DIR}/memory/linear_page_allocator_test.cpp
    ${POMDOG_TEST_DIR}/memory/memcpy_span_test.cpp
    ${POMDOG_TEST_DIR}/memory/memset_span_test.cpp
    ${POMDOG_TEST_DIR}/memory/placement_new_test.cpp
    ${POMDOG_TEST_DIR}/memory/raw_ptr_test.cpp
    ${POMDOG_TEST_DIR}/memory/unsafe_ptr_test.cpp
    ${POMDOG_TEST_DIR}/utility/bit_cast_span_test.cpp
    ${POMDOG_TEST_DIR}/utility/crc32_test.cpp
    ${POMDOG_TEST_DIR}/utility/errors_test.cpp
    ${POMDOG_TEST_DIR}/utility/path_helper_test.cpp
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # TODO: Need to implement for Emscripten.
        ${POMDOG_TEST_DIR}/utility/spin_lock_test.cpp
    >
    ${POMDOG_TEST_DIR}/utility/string_helper_test.cpp
    ${POMDOG_TEST_DIR}/testing/main.cpp
    ${POMDOG_TEST_DIR}/testing/testing.h
)

set_target_properties(pomdog_tests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(pomdog_tests PRIVATE
    ${POMDOG_DIR}
    ${POMDOG_DIR}/thirdparty/doctest
)

target_compile_definitions(pomdog_tests PRIVATE
    $<$<CONFIG:Debug>:_DEBUG;POMDOG_DEBUG_BUILD=1>
    $<$<CONFIG:Release>:NDEBUG>

    # On Windows
    $<$<PLATFORM_ID:Windows>:
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
)

target_compile_options(pomdog_tests PRIVATE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wall
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
        -Weverything
        -Wextra
        -Wmost
        -Wpedantic
        -Wcomma
        -Wnewline-eof
        -Wold-style-cast
        -Wundef
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++98-compat-bind-to-temporary-copy
        -Wno-c++98-compat-local-type-template-args
        -Wno-c++20-compat
        -Wno-c++20-compat-pedantic
        -Wno-padded

        # NOTE: Suppress several warnings for unit testing.
        -Wno-float-equal
    >
    $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>,$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,21>>:
        # NOTE: Disable thread safety analysis for Linux with Clang >= 21, because it causes false positives.
        -Wno-thread-safety-negative
    >
    # Apple Clang
    $<$<CXX_COMPILER_ID:AppleClang>:
        -Wempty-init-stmt
        -Wexit-time-destructors
        -Wextra-semi
        -Wextra-semi-stmt
        -Wimplicit-fallthrough
        -Wnoexcept-type
        -Wnon-virtual-dtor
        -Wnullability-extension
        -Woverloaded-virtual
        -Woverriding-method-mismatch
        -Wsign-conversion
        -Wstring-conversion
        -Wsuggest-destructor-override
        -Wsuggest-override
        -Wthread-safety
        -Wunneeded-internal-declaration
        -Wunneeded-member-function
    >
    # NOTE: GCC
    $<$<CXX_COMPILER_ID:GNU>:
        -Wimplicit-fallthrough
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        /Wall
        /WX
        $<$<CONFIG:Release>:/wd4710> # NOTE: Disable warning C4710 for function not inlined
        /wd4820 # NOTE: Disable warning C4820 for padding
        /wd5045 # NOTE: Disable warning C4820 for Spectre mitigation with /Qspectre
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
        -pthread

        # NOTE: Disable thread safety analysis for Emscripten, because it causes false positives.
        -Wno-thread-safety-negative
    >
)

target_link_options(pomdog_tests PRIVATE
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
        "SHELL:-s WASM=1"
        "SHELL:-s USE_PTHREADS=1"
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

set_target_properties(pomdog_tests PROPERTIES
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c11"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2b"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"

    # Warnings (Clang)
    XCODE_ATTRIBUTE_CLANG_WARN_BOOL_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_CONSTANT_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_EMPTY_BODY "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_ENUM_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_INT_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_UNGUARDED_AVAILABILITY "YES_AGGRESSIVE"
    XCODE_ATTRIBUTE_CLANG_WARN_UNREACHABLE_CODE "YES"

    # Warnings (GCC and Clang)
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

if(POMDOG_USE_ADDRESS_SANITIZER)
    set_target_properties(pomdog_tests PROPERTIES
        # Address Sanitizer
        XCODE_ATTRIBUTE_CLANG_ADDRESS_SANITIZER_CONTAINER_OVERFLOW "YES"

        # Undefined Sanitizer
        XCODE_ATTRIBUTE_CLANG_UNDEFINED_BEHAVIOR_SANITIZER_INTEGER "YES"
        XCODE_ATTRIBUTE_CLANG_UNDEFINED_BEHAVIOR_SANITIZER_NULLABILITY "YES"
    )
endif()

include(${POMDOG_DIR}/cmake/pomdog/asan.cmake)
pomdog_copy_asan_runtime(pomdog_tests)

if(NOT POMDOG_BUILD_TARGET_ALL)
    add_subdirectory(${POMDOG_DIR}/cmake/pomdog ${CMAKE_CURRENT_BINARY_DIR}/pomdog_build)
endif()
target_link_libraries(pomdog_tests pomdog_static)
