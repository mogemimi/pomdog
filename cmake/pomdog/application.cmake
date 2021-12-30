source_group(application        REGULAR_EXPRESSION "pomdog/application/*")
source_group(application/cocoa  REGULAR_EXPRESSION "pomdog/application/cocoa/*")
source_group(application/linux  REGULAR_EXPRESSION "pomdog/application/linux/*")
source_group(application/win32  REGULAR_EXPRESSION "pomdog/application/win32/*")
source_group(application/x11    REGULAR_EXPRESSION "pomdog/application/x11/*")

target_sources(pomdog_static PRIVATE
    # NOTE: application
    ${POMDOG_SRC_DIR}/application/game.h
    ${POMDOG_SRC_DIR}/application/game.cpp
    ${POMDOG_SRC_DIR}/application/game_host.h
    ${POMDOG_SRC_DIR}/application/game_host.cpp
    ${POMDOG_SRC_DIR}/application/game_window.h
    ${POMDOG_SRC_DIR}/application/game_window.cpp
    ${POMDOG_SRC_DIR}/application/mouse_cursor.h
    ${POMDOG_SRC_DIR}/application/subsystem_scheduler.h
    ${POMDOG_SRC_DIR}/application/system_events.h

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_cocoa.mm
        ${POMDOG_SRC_DIR}/application/cocoa/game_window_cocoa.h
        ${POMDOG_SRC_DIR}/application/cocoa/game_window_cocoa.mm
    >

    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_opengl_view.h
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_opengl_view.mm
    >

    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_METAL}>>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_metal.h
        ${POMDOG_SRC_DIR}/application/cocoa/game_host_metal.mm
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_metal_view_controller.h
        ${POMDOG_SRC_DIR}/application/cocoa/pomdog_metal_view_controller.mm
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: platform/linux
        ${POMDOG_SRC_DIR}/application/linux/game_host_linux.cpp
        ${POMDOG_SRC_DIR}/application/linux/game_host_linux.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: platform/x11
        ${POMDOG_SRC_DIR}/application/x11/game_window_x11.cpp
        ${POMDOG_SRC_DIR}/application/x11/game_window_x11.h
        ${POMDOG_SRC_DIR}/application/x11/x11_atom_cache.h
        ${POMDOG_SRC_DIR}/application/x11/x11_context.cpp
        ${POMDOG_SRC_DIR}/application/x11/x11_context.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: platform/win32
        ${POMDOG_SRC_DIR}/application/win32/dark_mode.cpp
        ${POMDOG_SRC_DIR}/application/win32/dark_mode.h
        ${POMDOG_SRC_DIR}/application/win32/game_host_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/game_host_win32.h
        ${POMDOG_SRC_DIR}/application/win32/game_window_win32.cpp
        ${POMDOG_SRC_DIR}/application/win32/game_window_win32.h
    >
)
