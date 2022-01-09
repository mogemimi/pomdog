cmake_minimum_required(VERSION 3.20)
project(feature_showcase CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(PRODUCT_NAME feature_showcase)
set(POMDOG_DIR "../..")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(source                             REGULAR_EXPRESSION "source/*")
source_group(source/animation_graph_test        REGULAR_EXPRESSION "source/animation_graph_test/*")
source_group(source/audio_clip_test             REGULAR_EXPRESSION "source/audio_clip_test/*")
source_group(source/basic_effect_test           REGULAR_EXPRESSION "source/basic_effect_test/*")
source_group(source/beam2d_test                 REGULAR_EXPRESSION "source/beam2d_test/*")
source_group(source/billboard_batch_test        REGULAR_EXPRESSION "source/billboard_batch_test/*")
source_group(source/distance_field_font_test    REGULAR_EXPRESSION "source/distance_field_font_test/*")
source_group(source/editor_gui_test             REGULAR_EXPRESSION "source/editor_gui_test/*")
source_group(source/gamepad_test                REGULAR_EXPRESSION "source/gamepad_test/*")
source_group(source/gif_decode_test             REGULAR_EXPRESSION "source/gif_decode_test/*")
source_group(source/gltf_model_test             REGULAR_EXPRESSION "source/gltf_model_test/*")
source_group(source/gui_splitter_test           REGULAR_EXPRESSION "source/gui_splitter_test/*")
source_group(source/hardware_instancing_test    REGULAR_EXPRESSION "source/hardware_instancing_test/*")
source_group(source/http_client_test            REGULAR_EXPRESSION "source/http_client_test/*")
source_group(source/image_effects_test          REGULAR_EXPRESSION "source/image_effects_test/*")
source_group(source/line_batch_test             REGULAR_EXPRESSION "source/line_batch_test/*")
source_group(source/multi_render_target_test    REGULAR_EXPRESSION "source/multi_render_target_test/*")
source_group(source/particle_clip_loader_test   REGULAR_EXPRESSION "source/particle_clip_loader_test/*")
source_group(source/particle2d_test             REGULAR_EXPRESSION "source/particle2d_test/*")
source_group(source/particle3d_test             REGULAR_EXPRESSION "source/particle3d_test/*")
source_group(source/polyline_drawing_test       REGULAR_EXPRESSION "source/polyline_drawing_test/*")
source_group(source/primitive_batch_test        REGULAR_EXPRESSION "source/primitive_batch_test/*")
source_group(source/skeletal2d_test             REGULAR_EXPRESSION "source/skeletal2d_test/*")
source_group(source/skinning2d_test             REGULAR_EXPRESSION "source/skinning2d_test/*")
source_group(source/sprite_batch_test           REGULAR_EXPRESSION "source/sprite_batch_test/*")
source_group(source/sprite_font_test            REGULAR_EXPRESSION "source/sprite_font_test/*")
source_group(source/sprite_line_test            REGULAR_EXPRESSION "source/sprite_line_test/*")
source_group(source/svg_decode_test             REGULAR_EXPRESSION "source/svg_decode_test/*")
source_group(source/texture2d_loader_test       REGULAR_EXPRESSION "source/texture2d_loader_test/*")
source_group(source/voxel_model_test            REGULAR_EXPRESSION "source/voxel_model_test/*")
source_group(platform/cocoa                     REGULAR_EXPRESSION "platform/cocoa/*")
source_group(platform/linux                     REGULAR_EXPRESSION "platform/linux/*")
source_group(platform/win32                     REGULAR_EXPRESSION "platform/win32/*")

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
    source/animation_graph_test/animation_graph_test.cpp
    source/animation_graph_test/animation_graph_test.h
    source/audio_clip_test/audio_clip_test.cpp
    source/audio_clip_test/audio_clip_test.h
    source/basic_effect_test/basic_effect_test.cpp
    source/basic_effect_test/basic_effect_test.h
    source/beam2d_test/beam2d_test.cpp
    source/beam2d_test/beam2d_test.h
    source/billboard_batch_test/billboard_batch_test.cpp
    source/billboard_batch_test/billboard_batch_test.h
    source/distance_field_font_test/distance_field_font_test.cpp
    source/distance_field_font_test/distance_field_font_test.h
    source/editor_gui_test/editor_gui_test.cpp
    source/editor_gui_test/editor_gui_test.h
    source/gamepad_test/gamepad_test.cpp
    source/gamepad_test/gamepad_test.h
    source/gif_decode_test/gif_decode_test.cpp
    source/gif_decode_test/gif_decode_test.h
    source/gltf_model_test/gltf_model_test.cpp
    source/gltf_model_test/gltf_model_test.h
    source/gui_splitter_test/gui_splitter_test.cpp
    source/gui_splitter_test/gui_splitter_test.h
    source/hardware_instancing_test/hardware_instancing_test.cpp
    source/hardware_instancing_test/hardware_instancing_test.h
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # TODO: Add emscripten support
        source/http_client_test/http_client_test.cpp
        source/http_client_test/http_client_test.h
    >
    source/image_effects_test/image_effects_test.cpp
    source/image_effects_test/image_effects_test.h
    source/line_batch_test/line_batch_test.cpp
    source/line_batch_test/line_batch_test.h
    source/multi_render_target_test/multi_render_target_test.cpp
    source/multi_render_target_test/multi_render_target_test.h
    source/particle_clip_loader_test/particle_clip_loader_test.cpp
    source/particle_clip_loader_test/particle_clip_loader_test.h
    source/particle2d_test/particle2d_test.cpp
    source/particle2d_test/particle2d_test.h
    source/particle3d_test/particle3d_test.cpp
    source/particle3d_test/particle3d_test.h
    source/polyline_drawing_test/polyline_drawing_test.cpp
    source/polyline_drawing_test/polyline_drawing_test.h
    source/primitive_batch_test/primitive_batch_test.cpp
    source/primitive_batch_test/primitive_batch_test.h
    source/skeletal2d_test/skeletal2d_test.cpp
    source/skeletal2d_test/skeletal2d_test.h
    source/skinning2d_test/skinning2d_test.cpp
    source/skinning2d_test/skinning2d_test.h
    source/sprite_batch_test/sprite_batch_test.cpp
    source/sprite_batch_test/sprite_batch_test.h
    source/sprite_font_test/sprite_font_test.cpp
    source/sprite_font_test/sprite_font_test.h
    source/sprite_line_test/sprite_line_test.cpp
    source/sprite_line_test/sprite_line_test.h
    source/svg_decode_test/svg_decode_test.cpp
    source/svg_decode_test/svg_decode_test.h
    source/texture2d_loader_test/texture2d_loader_test.cpp
    source/texture2d_loader_test/texture2d_loader_test.h
    source/voxel_model_test/voxel_model_test.cpp
    source/voxel_model_test/voxel_model_test.h
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
    CXX_STANDARD 20
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
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++20"
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

if(NOT POMDOG_BUILD_TARGET_ALL)
    add_subdirectory(${POMDOG_DIR}/cmake/pomdog ${CMAKE_CURRENT_BINARY_DIR}/pomdog_build)
endif()
target_link_libraries(${PRODUCT_NAME} pomdog_static)
