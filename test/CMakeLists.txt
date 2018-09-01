cmake_minimum_required(VERSION 3.10)
project(PomdogTest CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_DIR "..")
set(POMDOG_TEST_DIR "${POMDOG_DIR}/test")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(Application            REGULAR_EXPRESSION test/Application/*)
source_group(Async                  REGULAR_EXPRESSION test/Async/*)
source_group(Content                REGULAR_EXPRESSION test/Content/*)
source_group(Experimental/Gameplay  REGULAR_EXPRESSION test/Experimental/Gameplay/*)
source_group(Graphics               REGULAR_EXPRESSION test/Graphics/*)
source_group(Input                  REGULAR_EXPRESSION test/Input/*)
source_group(Logging                REGULAR_EXPRESSION test/Logging/*)
source_group(Math                   REGULAR_EXPRESSION test/Math/*)
source_group(Signals                REGULAR_EXPRESSION test/Signals/*)
source_group(Utility                REGULAR_EXPRESSION test/Utility/*)

add_executable(PomdogTest
  ${POMDOG_TEST_DIR}/main.cpp
  ${POMDOG_TEST_DIR}/Application/GameClockTest.cpp
  ${POMDOG_TEST_DIR}/Application/TimerTest.cpp
  ${POMDOG_TEST_DIR}/Content/PathHelperTest.cpp
  ${POMDOG_TEST_DIR}/Async/SchedulerTest.cpp
  ${POMDOG_TEST_DIR}/Async/TaskTest.cpp

  ${POMDOG_TEST_DIR}/Experimental/Gameplay/EntityIDTest.cpp
  ${POMDOG_TEST_DIR}/Experimental/Gameplay/EntityTest.cpp
  ${POMDOG_TEST_DIR}/Experimental/Gameplay/EntityManagerTest.cpp

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
  ${POMDOG_TEST_DIR}/Signals/ConnectionTest.cpp
  ${POMDOG_TEST_DIR}/Signals/ConnectionListTest.cpp
  ${POMDOG_TEST_DIR}/Signals/EventQueueTest.cpp
  ${POMDOG_TEST_DIR}/Signals/EventTest.cpp
  ${POMDOG_TEST_DIR}/Signals/HelpersTest.cpp
  ${POMDOG_TEST_DIR}/Signals/ScopedConnectionTest.cpp
  ${POMDOG_TEST_DIR}/Signals/SignalTest.cpp
  ${POMDOG_TEST_DIR}/Utility/AnyTest.cpp
  ${POMDOG_TEST_DIR}/Utility/CRC32Test.cpp
  ${POMDOG_TEST_DIR}/Utility/StringHelperTest.cpp
)

target_include_directories(PomdogTest PRIVATE
  ${POMDOG_DIR}/include
  ${POMDOG_DIR}/experimental
  ${POMDOG_DIR}/dependencies/iutest/include
)

target_compile_definitions(PomdogTest PRIVATE
  IUTEST_CHECK_STRICT=0
  $<$<CONFIG:DEBUG>:_DEBUG;DEBUG=1>
  $<$<CONFIG:RELEASE>:NDEBUG>

  # On Windows
  $<$<PLATFORM_ID:Windows>:
    WIN32_LEAN_AND_MEAN
    NOMINMAX
  >
)

target_compile_options(PomdogTest PRIVATE
  # Clang
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wcomma
    -Wmost
    -Wmove
    -Wimplicit-fallthrough
  >
  # GCC
  $<$<CXX_COMPILER_ID:GNU>:
    -Wimplicit-fallthrough
  >
  # Clang and GCC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${CMAKE_GENERATOR},Xcode>>:
    -Wall
    -Werror
    $<$<CONFIG:DEBUG>:-g;-O0>
    $<$<CONFIG:RELEASE>:-O3>
  >
  # MSVC
  $<$<CXX_COMPILER_ID:MSVC>:
    /W4
    /WX
    $<$<CONFIG:DEBUG>:/Od;/MTd>
    $<$<CONFIG:RELEASE>:/O2;/Ob2;/MT>
  >
)

if(APPLE)
  # TODO: Replace with the following, CMake >= 3.12
  # `target_link_options(PomdogTest PUBLIC "${LINK_FRAMEWORKS}")`
  # https://cmake.org/cmake/help/git-stage/command/target_link_options.html
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework Metal")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework AudioToolBox")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework OpenAL")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework Cocoa")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework OpenGL")
  set(LINK_FRAMEWORKS "${LINK_FRAMEWORKS} -framework QuartzCore")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FRAMEWORKS}")
endif()

if(WIN32)
  # Replace with target_link_options() when upgrading CMake 3.12
  target_link_libraries(PomdogTest
    debug -INCREMENTAL
    debug -DEBUG
    optimized -INCREMENTAL:NO
    optimized -OPT:REF
  )
endif()

set_target_properties(
  PomdogTest PROPERTIES
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
  XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
  XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
  XCODE_ATTRIBUTE_OTHER_CFLAGS[variant=Debug] "-g"

  XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
  XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17"
  XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
  XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.11"

  # Warnings (Clang)
  XCODE_ATTRIBUTE_CLANG_WARN_BOOL_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_CONSTANT_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_EMPTY_BODY "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_ENUM_CONVERSION "YES"
  XCODE_ATTRIBUTE_CLANG_WARN_INT_CONVERSION "YES"
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
  XCODE_ATTRIBUTE_CLANG_WARN_OBJC_ROOT_CLASS "YES_ERROR"

  # Warning Policies
  XCODE_ATTRIBUTE_GCC_TREAT_WARNINGS_AS_ERRORS "YES"
  XCODE_ATTRIBUTE_WARNING_CFLAGS "-Wall"

  # Symbols
  XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
  XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES" # -fvisibility-inlines-hidden
  XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES" # -fvisibility=hidden
)

if(NOT POMDOG_BUILD_TARGET_ALL)
  add_subdirectory("${POMDOG_DIR}/build/pomdog_experimental" "${CMAKE_CURRENT_BINARY_DIR}/pomdog_experimental_build")
endif()
target_link_libraries(PomdogTest pomdog_experimental)
