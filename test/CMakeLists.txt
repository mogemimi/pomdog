cmake_minimum_required(VERSION 3.20)
project(pomdog_test CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_DIR "..")
set(POMDOG_TEST_DIR "${POMDOG_DIR}/test")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(application            REGULAR_EXPRESSION test/application/*)
source_group(content                REGULAR_EXPRESSION test/content/*)
source_group(experimental\\async    REGULAR_EXPRESSION test/experimental/async/*)
source_group(experimental\\ecs      REGULAR_EXPRESSION test/experimental/ecs/*)
source_group(experimental\\random   REGULAR_EXPRESSION test/experimental/random/*)
source_group(graphics               REGULAR_EXPRESSION test/graphics/*)
source_group(input                  REGULAR_EXPRESSION test/input/*)
source_group(logging                REGULAR_EXPRESSION test/logging/*)
source_group(math                   REGULAR_EXPRESSION test/math/*)
source_group(network                REGULAR_EXPRESSION test/network/*)
source_group(signals                REGULAR_EXPRESSION test/signals/*)
source_group(utility                REGULAR_EXPRESSION test/utility/*)

add_executable(pomdog_test)

target_sources(pomdog_test PRIVATE
    ${POMDOG_TEST_DIR}/main.cpp
    ${POMDOG_TEST_DIR}/pomdog_test.cpp
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # TODO: Need to implement for Emscripten.
        ${POMDOG_TEST_DIR}/application/file_system_test.cpp
    >
    ${POMDOG_TEST_DIR}/application/game_clock_test.cpp
    ${POMDOG_TEST_DIR}/application/timer_test.cpp
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # TODO: Need to implement for Emscripten.
        ${POMDOG_TEST_DIR}/experimental/async/scheduler_test.cpp
        ${POMDOG_TEST_DIR}/experimental/async/task_test.cpp
    >
    ${POMDOG_TEST_DIR}/experimental/ecs/entity_test.cpp
    ${POMDOG_TEST_DIR}/experimental/ecs/entity_manager_test.cpp
    ${POMDOG_TEST_DIR}/experimental/random/xoroshiro128_star_star_test.cpp
    ${POMDOG_TEST_DIR}/graphics/input_layout_helper_test.cpp
    ${POMDOG_TEST_DIR}/input/gamepad_uuid_test.cpp
    ${POMDOG_TEST_DIR}/input/keyboard_state_test.cpp
    ${POMDOG_TEST_DIR}/input/keys_test.cpp
    ${POMDOG_TEST_DIR}/input/mouse_state_test.cpp
    ${POMDOG_TEST_DIR}/logging/log_channel_test.cpp
    ${POMDOG_TEST_DIR}/logging/log_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_box_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_box2d_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_circle_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_frustum_test.cpp
    ${POMDOG_TEST_DIR}/math/bounding_sphere_test.cpp
    ${POMDOG_TEST_DIR}/math/color_test.cpp
    ${POMDOG_TEST_DIR}/math/math_test.cpp
    ${POMDOG_TEST_DIR}/math/matrix2x2_test.cpp
    ${POMDOG_TEST_DIR}/math/matrix3x2_test.cpp
    ${POMDOG_TEST_DIR}/math/matrix3x3_test.cpp
    ${POMDOG_TEST_DIR}/math/matrix4x4_test.cpp
    ${POMDOG_TEST_DIR}/math/plane_test.cpp
    ${POMDOG_TEST_DIR}/math/point2d_test.cpp
    ${POMDOG_TEST_DIR}/math/point3d_test.cpp
    ${POMDOG_TEST_DIR}/math/quaternion_test.cpp
    ${POMDOG_TEST_DIR}/math/ray_test.cpp
    ${POMDOG_TEST_DIR}/math/rectangle_test.cpp
    ${POMDOG_TEST_DIR}/math/vector2_test.cpp
    ${POMDOG_TEST_DIR}/math/vector3_test.cpp
    ${POMDOG_TEST_DIR}/math/vector4_test.cpp
    ${POMDOG_TEST_DIR}/network/array_view_test.cpp
    ${POMDOG_TEST_DIR}/network/executor.hpp
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # TODO: Need to implement for Emscripten.
        ${POMDOG_TEST_DIR}/network/http_client_test.cpp
        ${POMDOG_TEST_DIR}/network/tcp_stream_test.cpp
        ${POMDOG_TEST_DIR}/network/tls_stream_test.cpp
        ${POMDOG_TEST_DIR}/network/udp_stream_test.cpp
    >
    ${POMDOG_TEST_DIR}/signals/connection_test.cpp
    ${POMDOG_TEST_DIR}/signals/connection_list_test.cpp
    ${POMDOG_TEST_DIR}/signals/delegate_test.cpp
    ${POMDOG_TEST_DIR}/signals/event_queue_test.cpp
    ${POMDOG_TEST_DIR}/signals/scoped_connection_test.cpp
    ${POMDOG_TEST_DIR}/signals/signal_helpers_test.cpp
    ${POMDOG_TEST_DIR}/signals/signal_test.cpp
    ${POMDOG_TEST_DIR}/utility/crc32_test.cpp
    ${POMDOG_TEST_DIR}/utility/errors_test.cpp
    ${POMDOG_TEST_DIR}/utility/path_helper_test.cpp
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # TODO: Need to implement for Emscripten.
        ${POMDOG_TEST_DIR}/utility/spin_lock_test.cpp
    >
    ${POMDOG_TEST_DIR}/utility/string_helper_test.cpp
)

set_target_properties(pomdog_test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(pomdog_test PRIVATE
    ${POMDOG_DIR}
    ${POMDOG_DIR}/dependencies/Catch2/extras
)

target_compile_definitions(pomdog_test PRIVATE
    $<$<CONFIG:Debug>:_DEBUG;DEBUG=1>
    $<$<CONFIG:Release>:NDEBUG>

    # On Windows
    $<$<PLATFORM_ID:Windows>:
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
)

target_compile_options(pomdog_test PRIVATE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wall
        -Werror
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>
    >
    # NOTE: Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Wcomma
        -Wextra-semi
        -Wmost
        -Wmove
        -Wnon-virtual-dtor
        -Wimplicit-fallthrough
    >
    # NOTE: GCC
    $<$<CXX_COMPILER_ID:GNU>:
        -Wimplicit-fallthrough
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /WX
        /std:c++latest
        $<$<CONFIG:Debug>:/Od;/MTd>
        $<$<CONFIG:Release>:/O2;/Ob2;/MT>
        $<$<CONFIG:Release>:/GL> # NOTE: For LTO build
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        -pthread
    >
)

target_link_options(pomdog_test PRIVATE
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
        $<$<CONFIG:Debug>:
            /INCREMENTAL
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
)

set_target_properties(pomdog_test PROPERTIES
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c11"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++20"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.14"

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

add_subdirectory(${POMDOG_DIR}/cmake/dependencies/catch2 "${CMAKE_CURRENT_BINARY_DIR}/catch2_build")
target_link_libraries(pomdog_test catch2_static)

if(NOT POMDOG_BUILD_TARGET_ALL)
    add_subdirectory(${POMDOG_DIR}/cmake/pomdog ${CMAKE_CURRENT_BINARY_DIR}/pomdog_build)
endif()
target_link_libraries(pomdog_test pomdog_static)
