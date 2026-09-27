cmake_minimum_required(VERSION 3.31)
project(pong CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(PRODUCT_NAME pong)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

cmake_path(SET APP_DIR           NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}")
cmake_path(SET POMDOG_DIR        NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
cmake_path(SET BUILD_CONTENT_DIR NORMALIZE "${POMDOG_DIR}/build/${PRODUCT_NAME}/content")
cmake_path(SET BUILD_WORK_DIR    NORMALIZE "${POMDOG_DIR}/build/${PRODUCT_NAME}/work")
cmake_path(SET SHIPPING_DIR      NORMALIZE "${POMDOG_DIR}/build/${PRODUCT_NAME}/shipping")

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(${POMDOG_DIR}/cmake/pomdog ${CMAKE_CURRENT_BINARY_DIR}/pomdog_build)
    add_subdirectory(${POMDOG_DIR}/cmake/pomdog_app_settings ${CMAKE_CURRENT_BINARY_DIR}/pomdog_app_settings_build)
endif()
if(NOT TARGET pomdog::base OR NOT TARGET pomdog_app_settings)
    message(FATAL_ERROR "The parent project must add cmake/pomdog and cmake/pomdog_app_settings before this application.")
endif()

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
    source/game_setup.cpp
    source/game_setup.h
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

    # NOTE: Sources use headers, not C++ named modules (CMP0155).
    CXX_SCAN_FOR_MODULES OFF
)

target_include_directories(${PRODUCT_NAME} PRIVATE
    ${POMDOG_DIR}
    ${POMDOG_DIR}/thirdparty/fmt/include
    source
)

target_compile_options(${PRODUCT_NAME} PRIVATE
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Werror
    >
    # NOTE: Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        # NOTE: This warns when an initializer omits a field without a default member initializer,
        # even though the field is zero-initialized. Enable it per application instead of in
        # pomdog_app_settings_warnings.
        -Wmissing-field-initializers
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        /WX
    >
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
    "--archive-file ${SHIPPING_DIR}/windows/content.idx"
)
set_property(TARGET pong PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS
    "$<JOIN:${POMDOG_VS_DEBUGGER_COMMAND_ARGUMENTS}, >"
)

set_property(TARGET pong PROPERTY XCODE_SCHEME_ARGUMENTS
    "--assets-dir ${BUILD_CONTENT_DIR}"
    # "--appdata-dir ${BUILD_WORK_DIR}/savedata"
    "--archive-file ${SHIPPING_DIR}/macos/content.idx"
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
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c11"
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11.0"

    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/platform/cocoa/info.plist
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$(inherited) @loader_path @executable_path/../Frameworks"

    # Static Analyzer
    XCODE_ATTRIBUTE_CLANG_ANALYZER_LOCALIZABILITY_NONLOCALIZED "YES"

    # Symbols
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES" # -fvisibility-inlines-hidden
    XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES" # -fvisibility=hidden

    # Address Sanitizer
    XCODE_ATTRIBUTE_CLANG_ADDRESS_SANITIZER_CONTAINER_OVERFLOW "$<IF:$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>,YES,NO>"

    # Undefined Sanitizer
    XCODE_ATTRIBUTE_CLANG_UNDEFINED_BEHAVIOR_SANITIZER_INTEGER "$<IF:$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>,YES,NO>"
    XCODE_ATTRIBUTE_CLANG_UNDEFINED_BEHAVIOR_SANITIZER_NULLABILITY "$<IF:$<BOOL:${POMDOG_USE_ADDRESS_SANITIZER}>,YES,NO>"
)

include(${POMDOG_DIR}/cmake/pomdog/asan.cmake)
pomdog_copy_asan_runtime(${PRODUCT_NAME})

target_link_libraries(${PRODUCT_NAME} PRIVATE
    pomdog_app_settings
    pomdog::application_gpu
    pomdog::console
    pomdog::content_gpu
    pomdog::content_input
    pomdog::content_audio
    pomdog::experimental_image_effects
    pomdog::graphics
    pomdog::vfs
)
