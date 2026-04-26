source_group(application             REGULAR_EXPRESSION "pomdog/application/*")
source_group(application/backends    REGULAR_EXPRESSION "pomdog/application/backends/*")
source_group(application/cocoa       REGULAR_EXPRESSION "pomdog/application/cocoa/*")
source_group(application/emscripten  REGULAR_EXPRESSION "pomdog/application/emscripten/*")
source_group(application/linux       REGULAR_EXPRESSION "pomdog/application/linux/*")
source_group(application/win32       REGULAR_EXPRESSION "pomdog/application/win32/*")
source_group(application/x11         REGULAR_EXPRESSION "pomdog/application/x11/*")

target_sources(pomdog_static PRIVATE
    # NOTE: application
    ${POMDOG_SRC_DIR}/application/default_vfs_setup.h
    ${POMDOG_SRC_DIR}/application/default_vfs_setup.cpp
    ${POMDOG_SRC_DIR}/application/game.h
    ${POMDOG_SRC_DIR}/application/game.cpp
    ${POMDOG_SRC_DIR}/application/game_setup.h
    ${POMDOG_SRC_DIR}/application/game_setup.cpp
    ${POMDOG_SRC_DIR}/application/game_host.h
    ${POMDOG_SRC_DIR}/application/game_host.cpp
    ${POMDOG_SRC_DIR}/application/game_host_options.h
    ${POMDOG_SRC_DIR}/application/game_window.h
    ${POMDOG_SRC_DIR}/application/game_window.cpp
    ${POMDOG_SRC_DIR}/application/mouse_cursor.h
    ${POMDOG_SRC_DIR}/application/backends/subsystem_scheduler.h
    ${POMDOG_SRC_DIR}/application/backends/system_event_queue.h
    ${POMDOG_SRC_DIR}/application/backends/system_event_queue.cpp
    ${POMDOG_SRC_DIR}/application/backends/system_events.h

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: application/cocoa
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
        ${POMDOG_SRC_DIR}/application/win32/bootstrap_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/bootstrap_win32.h
        ${POMDOG_SRC_DIR}/application/win32/dark_mode.cpp
        ${POMDOG_SRC_DIR}/application/win32/dark_mode.h
        ${POMDOG_SRC_DIR}/application/win32/game_host_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/game_host_win32.h
        ${POMDOG_SRC_DIR}/application/win32/game_window_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/game_window_win32.h
    >

    $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/win32
        ${POMDOG_SRC_DIR}/application/win32/opengl_context_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/opengl_context_win32.h
    >
)
