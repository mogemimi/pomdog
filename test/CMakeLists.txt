cmake_minimum_required(VERSION 3.16)
project(PomdogTest CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_DIR "..")
set(POMDOG_TEST_DIR "${POMDOG_DIR}/test")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(Application            REGULAR_EXPRESSION test/Application/*)
source_group(Async                  REGULAR_EXPRESSION test/Async/*)
source_group(Content                REGULAR_EXPRESSION test/Content/*)
source_group(Experimental\\ECS      REGULAR_EXPRESSION test/Experimental/ECS/*)
source_group(Experimental\\Random   REGULAR_EXPRESSION test/Experimental/Random/*)
source_group(Graphics               REGULAR_EXPRESSION test/Graphics/*)
source_group(Input                  REGULAR_EXPRESSION test/Input/*)
source_group(Logging                REGULAR_EXPRESSION test/Logging/*)
source_group(Math                   REGULAR_EXPRESSION test/Math/*)
source_group(Network                REGULAR_EXPRESSION test/Network/*)
source_group(Signals                REGULAR_EXPRESSION test/Signals/*)
source_group(Utility                REGULAR_EXPRESSION test/Utility/*)

add_executable(PomdogTest)

target_sources(PomdogTest PRIVATE
    ${POMDOG_TEST_DIR}/main.cpp
    ${POMDOG_TEST_DIR}/PomdogTest.cpp
    ${POMDOG_TEST_DIR}/Application/GameClockTest.cpp
    ${POMDOG_TEST_DIR}/Application/TimerTest.cpp
    ${POMDOG_TEST_DIR}/Async/SchedulerTest.cpp
    ${POMDOG_TEST_DIR}/Async/TaskTest.cpp
    ${POMDOG_TEST_DIR}/Experimental/ECS/EntityTest.cpp
    ${POMDOG_TEST_DIR}/Experimental/ECS/EntityManagerTest.cpp
    ${POMDOG_TEST_DIR}/Experimental/Random/Xoroshiro128StarStarTest.cpp
    ${POMDOG_TEST_DIR}/Graphics/InputLayoutHelperTest.cpp
    ${POMDOG_TEST_DIR}/Input/GamepadUUIDTest.cpp
    ${POMDOG_TEST_DIR}/Input/KeyboardStateTest.cpp
    ${POMDOG_TEST_DIR}/Input/KeysTest.cpp
    ${POMDOG_TEST_DIR}/Input/MouseStateTest.cpp
    ${POMDOG_TEST_DIR}/Logging/LogChannelTest.cpp
    ${POMDOG_TEST_DIR}/Logging/LogTest.cpp
    ${POMDOG_TEST_DIR}/Math/BoundingBoxTest.cpp
    ${POMDOG_TEST_DIR}/Math/BoundingBox2DTest.cpp
    ${POMDOG_TEST_DIR}/Math/BoundingCircleTest.cpp
    ${POMDOG_TEST_DIR}/Math/BoundingFrustumTest.cpp
    ${POMDOG_TEST_DIR}/Math/BoundingSphereTest.cpp
    ${POMDOG_TEST_DIR}/Math/ColorTest.cpp
    ${POMDOG_TEST_DIR}/Math/MathHelperTest.cpp
    ${POMDOG_TEST_DIR}/Math/Matrix2x2Test.cpp
    ${POMDOG_TEST_DIR}/Math/Matrix3x2Test.cpp
    ${POMDOG_TEST_DIR}/Math/Matrix3x3Test.cpp
    ${POMDOG_TEST_DIR}/Math/Matrix4x4Test.cpp
    ${POMDOG_TEST_DIR}/Math/PlaneTest.cpp
    ${POMDOG_TEST_DIR}/Math/Point2DTest.cpp
    ${POMDOG_TEST_DIR}/Math/Point3DTest.cpp
    ${POMDOG_TEST_DIR}/Math/QuaternionTest.cpp
    ${POMDOG_TEST_DIR}/Math/RayTest.cpp
    ${POMDOG_TEST_DIR}/Math/RectangleTest.cpp
    ${POMDOG_TEST_DIR}/Math/Vector2Test.cpp
    ${POMDOG_TEST_DIR}/Math/Vector3Test.cpp
    ${POMDOG_TEST_DIR}/Math/Vector4Test.cpp
    ${POMDOG_TEST_DIR}/Network/ArrayViewTest.cpp
    ${POMDOG_TEST_DIR}/Network/Executor.hpp
    ${POMDOG_TEST_DIR}/Network/HTTPClientTest.cpp
    ${POMDOG_TEST_DIR}/Network/TCPStreamTest.cpp
    ${POMDOG_TEST_DIR}/Network/TLSStreamTest.cpp
    ${POMDOG_TEST_DIR}/Network/UDPStreamTest.cpp
    ${POMDOG_TEST_DIR}/Signals/ConnectionTest.cpp
    ${POMDOG_TEST_DIR}/Signals/ConnectionListTest.cpp
    ${POMDOG_TEST_DIR}/Signals/DelegateTest.cpp
    ${POMDOG_TEST_DIR}/Signals/EventQueueTest.cpp
    ${POMDOG_TEST_DIR}/Signals/HelpersTest.cpp
    ${POMDOG_TEST_DIR}/Signals/ScopedConnectionTest.cpp
    ${POMDOG_TEST_DIR}/Signals/SignalTest.cpp
    ${POMDOG_TEST_DIR}/Utility/CRC32Test.cpp
    ${POMDOG_TEST_DIR}/Utility/ErrorsTest.cpp
    ${POMDOG_TEST_DIR}/Utility/PathHelperTest.cpp
    ${POMDOG_TEST_DIR}/Utility/SpinLockTest.cpp
    ${POMDOG_TEST_DIR}/Utility/StringHelperTest.cpp
)

set_target_properties(PomdogTest PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(PomdogTest PRIVATE
    ${POMDOG_DIR}/include
    ${POMDOG_DIR}/dependencies/Catch2/single_include/catch2
)

target_compile_definitions(PomdogTest PRIVATE
    $<$<CONFIG:Debug>:_DEBUG;DEBUG=1>
    $<$<CONFIG:Release>:NDEBUG>

    # On Windows
    $<$<PLATFORM_ID:Windows>:
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
)

target_compile_options(PomdogTest PRIVATE
    # Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wall
        -Werror
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>
    >
    # Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Wcomma
        -Wextra-semi
        -Wmost
        -Wmove
        -Wnon-virtual-dtor
        -Wimplicit-fallthrough
    >
    # GCC
    $<$<CXX_COMPILER_ID:GNU>:
        -Wimplicit-fallthrough
    >
    # MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /WX
        /std:c++latest
        $<$<CONFIG:Debug>:/Od;/MTd>
        $<$<CONFIG:Release>:/O2;/Ob2;/MT>
        $<$<CONFIG:Release>:/GL> # NOTE: For LTO build
    >
)

target_link_options(PomdogTest PRIVATE
    # On Apple macOS
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

    # MSVC
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
)

set_target_properties(PomdogTest PROPERTIES
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c11"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17"
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

if(NOT POMDOG_BUILD_TARGET_ALL)
    add_subdirectory(${POMDOG_DIR}/cmake/pomdog ${CMAKE_CURRENT_BINARY_DIR}/pomdog_build)
endif()
target_link_libraries(PomdogTest pomdog_static)
