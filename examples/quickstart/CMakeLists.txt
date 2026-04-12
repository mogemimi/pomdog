cmake_minimum_required(VERSION 3.27)
project(quickstart CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(PRODUCT_NAME quickstart)
set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(POMDOG_DIR "../..")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(BUILD_CONTENT_DIR "${POMDOG_DIR}/build/quickstart/content")
cmake_path(ABSOLUTE_PATH BUILD_CONTENT_DIR NORMALIZE)

set(BUILD_WORK_DIR "${POMDOG_DIR}/build/quickstart/work")
cmake_path(ABSOLUTE_PATH BUILD_WORK_DIR NORMALIZE)

set(SHIPPING_DIR "${POMDOG_DIR}/build/quickstart/shipping")
cmake_path(ABSOLUTE_PATH SHIPPING_DIR NORMALIZE)

source_group(source REGULAR_EXPRESSION "source/*")
source_group(platform/cocoa REGULAR_EXPRESSION "platform/cocoa/*")
source_group(platform/emscripten REGULAR_EXPRESSION "platform/emscripten/*")
source_group(platform/linux REGULAR_EXPRESSION "platform/linux/*")
source_group(platform/win32 REGULAR_EXPRESSION "platform/win32/*")

set(RESOURCE_FILES
    # On Mac
    $<$<PLATFORM_ID:Darwin>:
        platform/cocoa/Assets.xcassets
        platform/cocoa/Base.lproj/MainMenu.xib
    >
)

add_executable(${PRODUCT_NAME} WIN32)

target_sources(${PRODUCT_NAME} PRIVATE
    source/game_main.cpp
    source/game_main.h
    ${RESOURCE_FILES}

    # On Mac
    $<$<PLATFORM_ID:Darwin>:
        platform/cocoa/AppDelegate.h
        platform/cocoa/AppDelegate.mm
        platform/cocoa/main.mm
    >

    # On Linux
    $<$<PLATFORM_ID:Linux>:
        platform/linux/main.cpp
    >

    # On Emscripten
    $<$<PLATFORM_ID:Emscripten>:
        platform/emscripten/main.cpp
    >

    # On Windows
    $<$<PLATFORM_ID:Windows>:
        platform/win32/game.rc
        platform/win32/main.cpp
        platform/win32/resource.h
    >
)

set_target_properties(${PRODUCT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PRODUCT_NAME} PRIVATE
    ${POMDOG_DIR}
    ${POMDOG_DIR}/thirdparty/fmt/include
    source
)

target_link_options(${PRODUCT_NAME} PRIVATE
    # NOTE: Emscripten app-specific link options
    $<$<PLATFORM_ID:Emscripten>:
        "SHELL:-s EXPORTED_FUNCTIONS=['_main','_malloc','_free','_pomdog_enter_full_screen','_pomdog_enter_soft_full_screen']"
        "SHELL:--pre-js ${CMAKE_CURRENT_SOURCE_DIR}/platform/emscripten/preload.js"
    >
)

set(POMDOG_VS_DEBUGGER_COMMAND_ARGUMENTS
    "--assets-dir ${BUILD_CONTENT_DIR}"
    # "--appdata-dir ${BUILD_WORK_DIR}/savedata"
    "--archive-file ${SHIPPING_DIR}/desktop/content.idx"
)
set_property(TARGET quickstart PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS
    "$<JOIN:${POMDOG_VS_DEBUGGER_COMMAND_ARGUMENTS}, >"
)

set_property(TARGET quickstart PROPERTY XCODE_SCHEME_ARGUMENTS
    "--assets-dir ${BUILD_CONTENT_DIR}"
    # "--appdata-dir ${BUILD_WORK_DIR}/savedata"
    "--archive-file ${SHIPPING_DIR}/desktop/content.idx"
)

if(APPLE)
    set_source_files_properties(
        ${RESOURCE_FILES}
        PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
    )

    set_target_properties(${PRODUCT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        RESOURCE "${RESOURCE_FILES}"
    )
endif()

set_target_properties(${PRODUCT_NAME} PROPERTIES
    # Change working directory in Visual Studio
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"

    # Xcode
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c11"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2b"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"

    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/platform/cocoa/info.plist
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$(inherited) @loader_path @executable_path/../Frameworks"

    # Warnings (Clang)
    XCODE_ATTRIBUTE_CLANG_WARN_BOOL_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_CONSTANT_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_EMPTY_BODY "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_ENUM_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_INT_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_UNGUARDED_AVAILABILITY "YES_AGGRESSIVE"
    XCODE_ATTRIBUTE_CLANG_WARN_UNREACHABLE_CODE "YES"

    # Warnings (GCC and Clang)
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
    set_target_properties(${PRODUCT_NAME} PROPERTIES
        # Address Sanitizer
        XCODE_ATTRIBUTE_CLANG_ADDRESS_SANITIZER_CONTAINER_OVERFLOW "YES"

        # Undefined Sanitizer
        XCODE_ATTRIBUTE_CLANG_UNDEFINED_BEHAVIOR_SANITIZER_INTEGER "YES"
        XCODE_ATTRIBUTE_CLANG_UNDEFINED_BEHAVIOR_SANITIZER_NULLABILITY "YES"
    )
endif()

include(${POMDOG_DIR}/cmake/pomdog/asan.cmake)
pomdog_copy_asan_runtime(${PRODUCT_NAME})

if(NOT POMDOG_BUILD_TARGET_ALL)
    add_subdirectory(${POMDOG_DIR}/cmake/pomdog_app_settings ${CMAKE_CURRENT_BINARY_DIR}/pomdog_app_settings_build)
    add_subdirectory(${POMDOG_DIR}/cmake/pomdog ${CMAKE_CURRENT_BINARY_DIR}/pomdog_build)
endif()
target_link_libraries(${PRODUCT_NAME} pomdog_app_settings)
target_link_libraries(${PRODUCT_NAME} pomdog_static)
