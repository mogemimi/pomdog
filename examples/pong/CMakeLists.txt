cmake_minimum_required(VERSION 3.27)
project(pong CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(PRODUCT_NAME pong)
set(POMDOG_DIR "../..")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(source   REGULAR_EXPRESSION "source/*")
source_group(platform/cocoa REGULAR_EXPRESSION "platform/cocoa/*")
source_group(platform/linux REGULAR_EXPRESSION "platform/linux/*")
source_group(platform/win32 REGULAR_EXPRESSION "platform/win32/*")

set(RESOURCE_FILES
    # `content` directory
    content

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
    source
)

target_compile_definitions(${PRODUCT_NAME} PRIVATE
    $<$<CONFIG:Debug>:_DEBUG;POMDOG_DEBUG_BUILD=1>
    $<$<CONFIG:Release>:NDEBUG>

    # On Windows
    $<$<PLATFORM_ID:Windows>:
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
)

target_compile_options(${PRODUCT_NAME} PRIVATE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Werror
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>
    >
    # NOTE: Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Wcomma
        -Wextra-semi
        -Wimplicit-fallthrough
        -Wmost
        -Wmove
        -Wnewline-eof
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
        $<$<CONFIG:Debug>:/Od;/MTd>
        $<$<CONFIG:Release>:/O2;/Ob2;/MT>
        $<$<CONFIG:Release>:/GL> # NOTE: For LTO build
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        -pthread
    >
)

target_link_options(${PRODUCT_NAME} PRIVATE
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

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Copying 'Content' directory into shippable directory after building executable
    add_custom_command(TARGET ${PRODUCT_NAME}
        POST_BUILD
        COMMENT "Copying contents to shippable directory after building executable"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/content
            $<TARGET_FILE_DIR:${PRODUCT_NAME}>/content
    )
endif()

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
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.14"

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

if(NOT POMDOG_BUILD_TARGET_ALL)
    add_subdirectory(${POMDOG_DIR}/cmake/pomdog ${CMAKE_CURRENT_BINARY_DIR}/pomdog_build)
endif()
target_link_libraries(${PRODUCT_NAME} pomdog_static)
