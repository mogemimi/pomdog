add_library(pomdog_application_gpu STATIC)
add_library(pomdog::application_gpu ALIAS pomdog_application_gpu)

target_link_libraries(pomdog_application_gpu
    PUBLIC
        pomdog_application
        pomdog_gpu
        pomdog_math
    PRIVATE
        pomdog_build_settings
        pomdog_input
        pomdog_audio
        pomdog_filesystem
        $<$<PLATFORM_ID:Windows,Linux,Darwin>:
            pomdog_network
        >
        $<$<PLATFORM_ID:Windows>:
            user32.lib
            gdi32.lib
            ole32.lib
            winmm.lib

            # NOTE: Dark mode theme
            UxTheme.lib
            Dwmapi.lib

            # NOTE: for GetFileVersionInfoSizeW()
            Version.lib
        >
        $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_X11}>>:
            X11
        >
        $<$<AND:$<PLATFORM_ID:Windows,Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
            glew_static
        >
        $<$<BOOL:${POMDOG_USE_VULKAN}>:
            Vulkan::Vulkan
        >
)

source_group(application/cocoa       REGULAR_EXPRESSION "pomdog/application/cocoa/*")
source_group(application/emscripten  REGULAR_EXPRESSION "pomdog/application/emscripten/*")
source_group(application/linux       REGULAR_EXPRESSION "pomdog/application/linux/*")
source_group(application/win32       REGULAR_EXPRESSION "pomdog/application/win32/*")
source_group(application/x11         REGULAR_EXPRESSION "pomdog/application/x11/*")

target_sources(pomdog_application_gpu PRIVATE
    # NOTE: application
    ${POMDOG_SRC_DIR}/application/display_metrics.h
    ${POMDOG_SRC_DIR}/application/game_host_options.h
    ${POMDOG_SRC_DIR}/application/game_window.h
    ${POMDOG_SRC_DIR}/application/game_window.cpp
    ${POMDOG_SRC_DIR}/application/graphics_backend_helper.h
    ${POMDOG_SRC_DIR}/application/graphics_backend_helper.cpp
    ${POMDOG_SRC_DIR}/application/high_dpi_settings.h
    ${POMDOG_SRC_DIR}/application/high_dpi_settings.cpp
    ${POMDOG_SRC_DIR}/application/mouse_cursor.h
    ${POMDOG_SRC_DIR}/application/backends/subsystem_scheduler.h
    ${POMDOG_SRC_DIR}/application/backends/system_event_queue.h
    ${POMDOG_SRC_DIR}/application/backends/system_event_queue.cpp
    ${POMDOG_SRC_DIR}/application/backends/system_events.h

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: application/cocoa
        ${POMDOG_SRC_DIR}/application/cocoa/keyboard_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/keyboard_cocoa.cpp
        ${POMDOG_SRC_DIR}/application/cocoa/mouse_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/mouse_cocoa.cpp
        ${POMDOG_SRC_DIR}/application/cocoa/bootstrap_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/bootstrap_cocoa.mm
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_cocoa.mm
        ${POMDOG_SRC_DIR}/application/cocoa/game_window_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/game_window_cocoa.mm
    >

    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_opengl_view.h
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_opengl_view.mm
        ${POMDOG_SRC_DIR}/application/cocoa/opengl_context_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/opengl_context_cocoa.mm
    >

    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_METAL}>>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_metal.h
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_metal.mm
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_metal_view_controller.h
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_metal_view_controller.mm
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: application/emscripten
        ${POMDOG_SRC_DIR}/application/emscripten/keyboard_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/keyboard_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/mouse_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/mouse_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/touchscreen_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/touchscreen_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/bootstrap_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/bootstrap_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/game_host_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/game_host_emscripten.h
        ${POMDOG_SRC_DIR}/application/emscripten/game_window_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/game_window_emscripten.h
    >

    $<$<AND:$<PLATFORM_ID:Emscripten>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/emscripten
        ${POMDOG_SRC_DIR}/application/emscripten/opengl_context_emscripten.cpp
        ${POMDOG_SRC_DIR}/application/emscripten/opengl_context_emscripten.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: application/linux
        ${POMDOG_SRC_DIR}/application/linux/game_host_linux.cpp
        ${POMDOG_SRC_DIR}/application/linux/game_host_linux.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: application/x11
        ${POMDOG_SRC_DIR}/application/x11/keyboard_x11.h
        ${POMDOG_SRC_DIR}/application/x11/keyboard_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/mouse_x11.h
        ${POMDOG_SRC_DIR}/application/x11/mouse_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/bootstrap_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/bootstrap_x11.h
        ${POMDOG_SRC_DIR}/application/x11/game_window_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/game_window_x11.h
        ${POMDOG_SRC_DIR}/application/x11/x11_atom_cache.h
        ${POMDOG_SRC_DIR}/application/x11/x11_context.cpp
        ${POMDOG_SRC_DIR}/application/x11/x11_context.h
    >

    $<$<AND:$<PLATFORM_ID:Linux>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/x11
        ${POMDOG_SRC_DIR}/application/x11/opengl_context_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/opengl_context_x11.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: application/win32
        ${POMDOG_SRC_DIR}/application/win32/keyboard_win32.h
        ${POMDOG_SRC_DIR}/application/win32/keyboard_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/mouse_win32.h
        ${POMDOG_SRC_DIR}/application/win32/mouse_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/bootstrap_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/bootstrap_win32.h
        ${POMDOG_SRC_DIR}/application/win32/dark_mode.cpp
        ${POMDOG_SRC_DIR}/application/win32/dark_mode.h
        ${POMDOG_SRC_DIR}/application/win32/game_host_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/game_host_win32.h
        ${POMDOG_SRC_DIR}/application/win32/game_window_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/game_window_win32.h
        ${POMDOG_SRC_DIR}/application/win32/precise_sleeper_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/precise_sleeper_win32.h
    >

    $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/win32
        ${POMDOG_SRC_DIR}/application/win32/opengl_context_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/opengl_context_win32.h
    >
)

target_link_options(pomdog_application_gpu INTERFACE
    $<$<PLATFORM_ID:Darwin>:
        "SHELL:-framework Cocoa"
        "SHELL:-framework QuartzCore"
    >
    $<$<PLATFORM_ID:Emscripten>:
        "SHELL:-s OFFSCREEN_FRAMEBUFFER=1"
    >
)

target_compile_definitions(pomdog_application_gpu PRIVATE
    $<$<BOOL:${POMDOG_USE_GL4}>:
        POMDOG_USE_GL4
    >
    $<$<BOOL:${POMDOG_USE_DIRECT3D11}>:
        POMDOG_USE_DIRECT3D11
    >
    $<$<BOOL:${POMDOG_USE_VULKAN}>:
        POMDOG_USE_VULKAN
    >
    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        GL_SILENCE_DEPRECATION # For silencing OpenGL warnings on macOS >= 10.14.
    >
)
