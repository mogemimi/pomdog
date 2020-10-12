cmake_minimum_required(VERSION 3.16)
project(Pomdog CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_DIR "../..")
set(POMDOG_INC_DIR "${POMDOG_DIR}/include/Pomdog")
set(POMDOG_SRC_DIR "${POMDOG_DIR}/src")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

source_group(Application                            REGULAR_EXPRESSION "(include/Pomdog|src)/Application/*")
source_group(Async                                  REGULAR_EXPRESSION "(include/Pomdog|src)/Async/*")
source_group(Audio                                  REGULAR_EXPRESSION "(include/Pomdog|src)/Audio/*")
source_group(Audio.OpenAL                           REGULAR_EXPRESSION "(include/Pomdog|src)/Audio.OpenAL/*")
source_group(Audio.XAudio2                          REGULAR_EXPRESSION "(include/Pomdog|src)/Audio.XAudio2/*")
source_group(Basic                                  REGULAR_EXPRESSION "(include/Pomdog|src)/Basic/*")
source_group(Content                                REGULAR_EXPRESSION "(include/Pomdog|src)/Content/*")
source_group(Content\\AssetBuilders                 REGULAR_EXPRESSION "(include/Pomdog|src)/Content/AssetBuilders/*")
source_group(Content\\AssetLoaders                  REGULAR_EXPRESSION "(include/Pomdog|src)/Content/AssetLoaders/*")
source_group(Content\\Audio                         REGULAR_EXPRESSION "(include/Pomdog|src)/Content/Audio/*")
source_group(Content\\Image                         REGULAR_EXPRESSION "(include/Pomdog|src)/Content/Image/*")
source_group(Content\\Utility                       REGULAR_EXPRESSION "(include/Pomdog|src)/Content/Utility/*")
source_group(Graphics                               REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics/*")
source_group(Graphics\\ShaderCompilers              REGULAR_EXPRESSION "(include/Pomdog|src)/Graphics/ShaderCompilers/*")
source_group(Input                                  REGULAR_EXPRESSION "(include/Pomdog|src)/Input/*")
source_group(Logging                                REGULAR_EXPRESSION "(include/Pomdog|src)/Logging/*")
source_group(Math                                   REGULAR_EXPRESSION "(include/Pomdog|src)/Math/*")
source_group(Network                                REGULAR_EXPRESSION "(include/Pomdog|src)/Network/*")
source_group(Platform.Apple                         REGULAR_EXPRESSION "(include/Pomdog/Platform/Apple|src/Platform.Apple)/*")
source_group(Platform.Cocoa                         REGULAR_EXPRESSION "(include/Pomdog/Platform/Cocoa|src/Platform.Cocoa)/*")
source_group(Platform.Linux                         REGULAR_EXPRESSION "(include/Pomdog/Platform/Linux|src/Platform.Linux)/*")
source_group(Platform.Win32                         REGULAR_EXPRESSION "(include/Pomdog/Platform/Win32|src/Platform.Win32)/*")
source_group(Platform.X11                           REGULAR_EXPRESSION "(include/Pomdog/Platform/X11|src/Platform.X11)/*")
source_group(Signals                                REGULAR_EXPRESSION "(include/Pomdog|src)/Signals/*")
source_group(Utility                                REGULAR_EXPRESSION "(include/Pomdog|src)/Utility/*")
source_group(Experimental\\ECS                      REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/ECS/*")
source_group(Experimental\\Graphics                 REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Graphics/*")
source_group(Experimental\\GUI                      REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/GUI/*")
source_group(Experimental\\ImageEffects             REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/ImageEffects/*")
source_group(Experimental\\Image                    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Image/*")
source_group(Experimental\\MagicaVoxel              REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/MagicaVoxel/*")
source_group(Experimental\\Particles                REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Particles/*")
source_group(Experimental\\Particles\\EmitterShapes REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Particles/EmitterShapes/*")
source_group(Experimental\\Particles\\Parameters    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Particles/Parameters/*")
source_group(Experimental\\Random                   REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Random/*")
source_group(Experimental\\Reactive                 REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Reactive/*")
source_group(Experimental\\Skeletal2D               REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Skeletal2D/*")
source_group(Experimental\\Skeletal2D\\BlendTrees   REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Skeletal2D/BlendTrees/*")
source_group(Experimental\\Skeletal2D\\Tracks       REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Skeletal2D/Tracks/*")
source_group(Experimental\\Spine                    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Spine/*")
source_group(Experimental\\TexturePacker            REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/TexturePacker/*")
source_group(Experimental\\Tween                    REGULAR_EXPRESSION "(include/Pomdog|src)/Experimental/Tween/*")
source_group(InputSystem                            REGULAR_EXPRESSION src/InputSystem/*)
source_group(InputSystem.DirectInput                REGULAR_EXPRESSION src/InputSystem.DirectInput/*)
source_group(InputSystem.IOKit                      REGULAR_EXPRESSION src/InputSystem.IOKit/*)
source_group(InputSystem.Linux                      REGULAR_EXPRESSION src/InputSystem.Linux/*)
source_group(Network.MbedTLS                        REGULAR_EXPRESSION src/Network.MbedTLS/*)
source_group(Network.POSIX                          REGULAR_EXPRESSION src/Network.POSIX/*)
source_group(Network.Win32                          REGULAR_EXPRESSION src/Network.Win32/*)
source_group(RenderSystem                           REGULAR_EXPRESSION src/RenderSystem/*)
source_group(RenderSystem.DXGI                      REGULAR_EXPRESSION src/RenderSystem.DXGI/*)
source_group(RenderSystem.Direct3D                  REGULAR_EXPRESSION src/RenderSystem.Direct3D/*)
source_group(RenderSystem.Direct3D11                REGULAR_EXPRESSION src/RenderSystem.Direct3D11/*)
source_group(RenderSystem.GL4                       REGULAR_EXPRESSION src/RenderSystem.GL4/*)
source_group(RenderSystem.Metal                     REGULAR_EXPRESSION src/RenderSystem.Metal/*)
source_group(RenderSystem.Vulkan                    REGULAR_EXPRESSION src/RenderSystem.Vulkan/*)

if(APPLE)
    set(POMDOG_TARGET_PLATFORM "Mac"   CACHE STRING "Choose platform")
    set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       true    CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       true    CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(POMDOG_TARGET_PLATFORM "Linux" CACHE STRING "Choose platform")
    set(POMDOG_USE_DIRECT3D11  false   CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      true    CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     false   CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT false   CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         true    CACHE BOOL   "Use X11")
elseif(WIN32)
    set(POMDOG_TARGET_PLATFORM "Win32" CACHE STRING "Choose platform")
    set(POMDOG_USE_DIRECT3D11  true    CACHE BOOL   "Use Direct3D11")
    set(POMDOG_USE_GL4         true    CACHE BOOL   "Use OpenGL4")
    set(POMDOG_USE_METAL       false   CACHE BOOL   "Use Metal")
    set(POMDOG_USE_VULKAN      false   CACHE BOOL   "Use Vulkan")
    set(POMDOG_USE_OPENAL      false   CACHE BOOL   "Use OpenAL")
    set(POMDOG_USE_XAUDIO2     true    CACHE BOOL   "Use XAudio2")
    set(POMDOG_USE_DIRECTINPUT true    CACHE BOOL   "Use DirectInput")
    set(POMDOG_USE_IOKIT       false   CACHE BOOL   "Use IOKit")
    set(POMDOG_USE_X11         false   CACHE BOOL   "Use X11")
endif()
set(POMDOG_USE_EXPERIMENTAL true CACHE BOOL "Use Experimental")

# NOTE: static library
add_library(pomdog_static STATIC)
include(PomdogApplication.cmake)
include(PomdogAsync.cmake)
include(PomdogAudio.cmake)
include(PomdogBasic.cmake)
include(PomdogContent.cmake)
include(PomdogExperimental.cmake)
include(PomdogGraphics.cmake)
include(PomdogInput.cmake)
include(PomdogInputSystem.cmake)
include(PomdogLogging.cmake)
include(PomdogMath.cmake)
include(PomdogNetwork.cmake)
include(PomdogPlatform.cmake)
include(PomdogRenderSystem.cmake)
include(PomdogSignals.cmake)
include(PomdogUtility.cmake)

set_target_properties(pomdog_static PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF

    $<$<BOOL:${UNIX}>:
        # On Unix platforms, the library is called `libpomdog.a`.
        OUTPUT_NAME pomdog
    >
)

add_subdirectory(${POMDOG_DIR}/cmake/dependencies/libpng "${CMAKE_CURRENT_BINARY_DIR}/libpng_build")
add_subdirectory(${POMDOG_DIR}/cmake/dependencies/giflib "${CMAKE_CURRENT_BINARY_DIR}/giflib_build")
add_subdirectory(${POMDOG_DIR}/cmake/dependencies/mbedtls "${CMAKE_CURRENT_BINARY_DIR}/mbedtls_build")
add_subdirectory(${POMDOG_DIR}/cmake/dependencies/stb "${CMAKE_CURRENT_BINARY_DIR}/stb_build")

if(POMDOG_USE_GL4 AND (POMDOG_TARGET_PLATFORM MATCHES "Linux"))
    add_subdirectory(${POMDOG_DIR}/cmake/dependencies/glew "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
endif()
if(POMDOG_USE_GL4 AND (POMDOG_TARGET_PLATFORM MATCHES "Win32"))
    add_subdirectory(${POMDOG_DIR}/cmake/dependencies/glew "${CMAKE_CURRENT_BINARY_DIR}/glew_build")
endif()

target_include_directories(pomdog_static PRIVATE
    ${POMDOG_DIR}/include
    ${POMDOG_DIR}/dependencies
    ${POMDOG_DIR}/dependencies/giflib
    ${POMDOG_DIR}/dependencies/libpng
    ${POMDOG_DIR}/dependencies/mbedtls/include
    ${POMDOG_DIR}/dependencies/rapidjson/include
    ${POMDOG_DIR}/dependencies/stb
    ${POMDOG_DIR}/dependencies/vendor/libpng
    ${POMDOG_DIR}/dependencies/vendor/SDL_GameControllerDB
    ${POMDOG_DIR}/dependencies/vendor/stb

    $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_X11}>>:
        /usr/X11R6/include
    >
    $<$<AND:$<PLATFORM_ID:Windows,Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
        ${POMDOG_DIR}/dependencies/vendor/glew/include
    >
)

target_link_libraries(pomdog_static INTERFACE
    png
    giflib
    mbedtls
    stb_static

    $<$<PLATFORM_ID:Linux>:
        # NOTE: Linux
        pthread
        $<$<BOOL:${POMDOG_USE_OPENAL}>: openal>
        $<$<BOOL:${POMDOG_USE_X11}>: X11>
        $<$<BOOL:${POMDOG_USE_GL4}>: GL>
    >
    $<$<PLATFORM_ID:Windows>:
        # NOTE: Windows
        kernel32.lib
        user32.lib
        gdi32.lib
        ole32.lib
        winmm.lib
        shell32.lib
        ws2_32.lib

        # NOTE: Dark mode theme
        UxTheme.lib
        Dwmapi.lib

        # NOTE: Windows version
        Version.lib # NOTE: for GetFileVersionInfoSizeW()

        $<$<BOOL:${POMDOG_USE_GL4}>:
            opengl32.lib # for OpenGL on Windows
        >
        $<$<BOOL:${POMDOG_USE_XAUDIO2}>:
            xaudio2.lib # for XAudio2
        >
        $<$<BOOL:${POMDOG_USE_DIRECT3D11}>:
            dxgi.lib
            d3d11.lib
            d3dcompiler.lib
            dxguid.lib # for _IID_ID3D11ShaderReflection
        >
        $<$<BOOL:${POMDOG_USE_DIRECTINPUT}>:
            dinput8.lib
            dxguid.lib
            oleaut32.lib # for SysAllocString
        >
    >
    $<$<AND:$<PLATFORM_ID:Windows,Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: GLEW for Windows and Linux
        glew_static
    >
)

target_link_directories(pomdog_static INTERFACE
    # X11 on Linux
    $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_X11}>>:
        /usr/X11R6/lib
    >
)

target_link_options(pomdog_static PRIVATE
    $<$<PLATFORM_ID:Darwin>:
        # NOTE: Apple macOS
        "SHELL: -framework Cocoa"
        "SHELL: -framework QuartzCore"
        $<$<BOOL:${POMDOG_USE_METAL}>:
            "SHELL: -framework Metal"
            "SHELL: -framework MetalKit"
        >
        $<$<BOOL:${POMDOG_USE_OPENAL}>:
            "SHELL: -framework AudioToolBox"
            "SHELL: -framework OpenAL"
        >
        $<$<BOOL:${POMDOG_USE_IOKIT}>:
            "SHELL: -framework IOKit"
        >
        $<$<BOOL:${POMDOG_USE_GL4}>:
            "SHELL: -framework OpenGL"
        >
    >
)

target_compile_definitions(pomdog_static PRIVATE
    $<$<CONFIG:Debug>:_DEBUG;DEBUG=1>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<PLATFORM_ID:Windows>:
        _WIN32_WINNT=0x0602 # Windows 8 or later
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    >
    $<$<AND:$<PLATFORM_ID:Windows,Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
        GLEW_STATIC
    >
    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        GL_SILENCE_DEPRECATION # For silencing OpenGL warnings on macOS >= 10.14.
    >
)

target_compile_options(pomdog_static PRIVATE
    # Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Wall
        -Werror
        -Wnon-virtual-dtor
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>

        # excluding clang-cl
        $<$<NOT:$<BOOL:MSVC>>:
            -pedantic
            -pedantic-errors
            -fvisibility=hidden
        >
    >
    # Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Wcomma
        -Wextra-semi
        -Wimplicit-fallthrough
        -Wmost
        -Wmove
        -Wnewline-eof

        # FIXME: Use -Wextra instead of the following options.
        -Wignored-qualifiers
        -Winitializer-overrides
        -Wmissing-field-initializers
        -Wmissing-method-return-type
        -Wsemicolon-before-method-body
        -Wsign-compare
        # -Wunused-parameter

        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++98-compat-bind-to-temporary-copy
        -Wno-c++98-compat-local-type-template-args
        -Wno-padded
    >
    # GCC
    $<$<CXX_COMPILER_ID:GNU>:
        # FIXME: Use -Wextra instead of the following options.
        -Wclobbered
        -Wcast-function-type
        -Wempty-body
        -Wignored-qualifiers
        -Wimplicit-fallthrough=3
        -Wmissing-field-initializers
        # -Wtype-limits
        -Wuninitialized
        -Wshift-negative-value
    >
    # MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        # NOTE: See https://docs.microsoft.com/en-us/cpp/preprocessor/compiler-warnings-that-are-off-by-default
        /W4
        /WX
        /we4263
        /we4264
        /we4266
        /we4287
        /we4289
        /we4412
        /we4431
        /we4686
        /we4928
        /we4987
        /std:c++latest
        /permissive-
        $<$<CONFIG:Debug>:/Od;/MTd>
        $<$<CONFIG:Release>:/O2;/Ob2;/MT>
    >
)

# NOTE: For dynamic library
# NOTE: see https://cmake.org/cmake/help/v3.9/prop_tgt/FRAMEWORK.html
# set_target_properties(pomdog_static PROPERTIES
#   XCODE_ATTRIBUTE_PRODUCT_NAME "Pomdog"
#   XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "net.enginetrouble.pomdog"
#   XCODE_ATTRIBUTE_INFOPLIST_FILE "src/Platform.Apple/Info.plist"
#   XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_LIBRARY_DIR)/Frameworks"
#   XCODE_ATTRIBUTE_DYLIB_INSTALL_NAME_BASE "@rpath"
#   XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$(inherited);@executable_path/../Frameworks;@loader_path/Frameworks"
# )

set_target_properties(pomdog_static PROPERTIES
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0" # -O0
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] "3" # -O3
    XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug] "YES"
    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "YES"

    XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0"
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c11"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++2a"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.14"

    XCODE_ATTRIBUTE_SKIP_INSTALL "YES"

    # Warnings (Clang)
    XCODE_ATTRIBUTE_CLANG_WARN_ASSIGN_ENUM "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_BOOL_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_COMMA "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_CONSTANT_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_DOCUMENTATION_COMMENTS "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_EMPTY_BODY "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_ENUM_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_INFINITE_RECURSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_INT_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_NON_LITERAL_NULL_CONVERSION "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_RANGE_LOOP_ANALYSIS "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_STRICT_PROTOTYPES "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_SUSPICIOUS_MOVE "YES"
    XCODE_ATTRIBUTE_CLANG_WARN_UNREACHABLE_CODE "YES"

    # Warnings (GCC and Clang)
    XCODE_ATTRIBUTE_GCC_TREAT_INCOMPATIBLE_POINTER_TYPE_WARNINGS_AS_ERRORS "YES"
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
