cmake_minimum_required(VERSION 3.20)
project(pomdog CXX)

# NOTE: Remove /RTC1 option from default compiler options for Visual Studio
STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

set(POMDOG_DIR "../..")
set(POMDOG_SRC_DIR "${POMDOG_DIR}/pomdog")
set(CMAKE_CONFIGURATION_TYPES Debug Release)

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
include(application.cmake)
include(audio.cmake)
include(basic.cmake)
include(chrono.cmake)
include(content.cmake)
include(experimental.cmake)
include(filesystem.cmake)
include(gpu.cmake)
include(input.cmake)
include(logging.cmake)
include(math.cmake)
include(memory.cmake)
include(network.cmake)
include(platform.cmake)
include(signals.cmake)
include(utility.cmake)

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
    ${POMDOG_DIR}
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
    $<$<PLATFORM_ID:Windows,Linux,Darwin>:
        # NOTE: MbedTLS for Windows, Linux and macOS
        mbedtls
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
    # NOTE: Clang and GCC
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
        -Werror
        $<$<CONFIG:Debug>:-g;-O0>
        $<$<CONFIG:Release>:-O3>

        # NOTE: excluding clang-cl
        $<$<NOT:$<BOOL:MSVC>>:
            -pedantic
            -pedantic-errors
            -fvisibility=hidden
        >
    >
    # NOTE: Clang
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++98-compat-bind-to-temporary-copy
        -Wno-c++98-compat-local-type-template-args
        -Wno-padded

        # FIXME: Remove the following `-Wno-...` flags.
        -Wno-float-equal
        -Wno-sign-conversion
        -Wno-switch-enum
        -Wno-undefined-func-template
        -Wno-implicit-int-float-conversion

        # FIXME: Conflict between C++20 aggregate initializations and C99 desinated initializers.
        -Wno-c99-extensions

        # FIXME: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#vague-itemplate
        # FIXME: https://bugs.llvm.org/show_bug.cgi?id=18733
        -Wno-weak-template-vtables

        $<$<CXX_COMPILER_ID:AppleClang>:
            # FIXME: Remove the following `-Wno` flags.
            -Wno-direct-ivar-access
            -Wno-objc-messaging-id
        >
    >
    # NOTE: GCC
    $<$<CXX_COMPILER_ID:GNU>:
        -Wall
        -Wextra
        -Wcast-align
        -Wcast-qual
        # -Wconversion
        -Wctor-dtor-privacy
        -Wdelete-non-virtual-dtor
        -Wdisabled-optimization
        -Wdouble-promotion
        -Wendif-labels
        -Wformat-nonliteral
        -Wformat-security
        -Wformat-signedness
        -Winit-self
        # -Winline
        -Wlogical-op
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wredundant-decls
        -Wreorder
        -Wshadow
        -Wstrict-null-sentinel
        # -Wsuggest-final-methods
        -Wundef
        -Wunsafe-loop-optimizations
        # -Wuseless-cast
        -Wvariadic-macros
        -Wwrite-strings
        -Wzero-as-null-pointer-constant
    >
    # NOTE: MSVC
    $<$<CXX_COMPILER_ID:MSVC>:
        # NOTE: See https://docs.microsoft.com/en-us/cpp/preprocessor/compiler-warnings-that-are-off-by-default
        /Wall
        /WX
        /wd4061 # NOTE: enumerator 'E::X' in switch of enum 'E' is not explicitly handled by a case label
        /wd4365 # NOTE: signed/unsigned mismatch
        /wd4464 # NOTE: relative include path contains '..'
        /wd4514 # NOTE: unreferenced inline function has been removed
        /wd4625 # NOTE: copy constructor was implicitly defined as deleted
        /wd4626 # NOTE: assignment operator was implicitly defined as deleted
        /wd4710 # NOTE: function not inlined
        /wd4711 # NOTE: function 'F' selected for automatic inline expansion
        /wd4820 # NOTE: 'N' bytes padding added after data member
        /wd5026 # NOTE: move constructor was implicitly defined as deleted
        /wd5027 # NOTE: move assignment operator was implicitly defined as deleted
        /wd5039 # NOTE: Undefined behavior may occur if this function throws an exception.
        /wd5045 # NOTE: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
        /wd5219 # NOTE: implicit conversion from 'int' to 'float', possible loss of data
        /std:c++latest
        /permissive-
        /utf-8
        $<$<CONFIG:Debug>:/Od;/MTd>
        $<$<CONFIG:Release>:/O2;/Ob2;/MT>
    >
    # NOTE: emcc (Emscripten Compiler Frontend)
    $<$<PLATFORM_ID:Emscripten>:
        -pthread
    >
)

# NOTE: For dynamic library
# NOTE: see https://cmake.org/cmake/help/v3.9/prop_tgt/FRAMEWORK.html
# set_target_properties(pomdog_static PROPERTIES
#   XCODE_ATTRIBUTE_PRODUCT_NAME "pomdog"
#   XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "net.enginetrouble.pomdog"
#   XCODE_ATTRIBUTE_INFOPLIST_FILE "Info.plist"
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
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++20"
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
