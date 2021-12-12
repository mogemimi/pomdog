source_group(platform/cocoa      REGULAR_EXPRESSION "pomdog/platform/cocoa/*")
source_group(platform/win32      REGULAR_EXPRESSION "pomdog/platform/win32/*")
source_group(platform/x11        REGULAR_EXPRESSION "pomdog/platform/x11/*")

target_sources(pomdog_static PRIVATE
    $<$<PLATFORM_ID:Darwin>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/platform/cocoa/bootstrap.hpp
        ${POMDOG_SRC_DIR}/platform/cocoa/bootstrap.mm
        ${POMDOG_SRC_DIR}/platform/cocoa/game_host_cocoa.hpp
        ${POMDOG_SRC_DIR}/platform/cocoa/game_host_cocoa.mm
        ${POMDOG_SRC_DIR}/platform/cocoa/game_window_cocoa.hpp
        ${POMDOG_SRC_DIR}/platform/cocoa/game_window_cocoa.mm
    >

    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/platform/cocoa/opengl_context_cocoa.hpp
        ${POMDOG_SRC_DIR}/platform/cocoa/opengl_context_cocoa.mm
        ${POMDOG_SRC_DIR}/platform/cocoa/pomdog_opengl_view.hpp
        ${POMDOG_SRC_DIR}/platform/cocoa/pomdog_opengl_view.mm
    >

    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_METAL}>>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/platform/cocoa/game_host_metal.hpp
        ${POMDOG_SRC_DIR}/platform/cocoa/game_host_metal.mm
        ${POMDOG_SRC_DIR}/platform/cocoa/pomdog_metal_view_controller.hpp
        ${POMDOG_SRC_DIR}/platform/cocoa/pomdog_metal_view_controller.mm
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: platform/x11
        ${POMDOG_SRC_DIR}/platform/x11/bootstrap.hpp
        ${POMDOG_SRC_DIR}/platform/x11/bootstrap.cpp
        ${POMDOG_SRC_DIR}/platform/x11/game_host_x11.cpp
        ${POMDOG_SRC_DIR}/platform/x11/game_host_x11.hpp
        ${POMDOG_SRC_DIR}/platform/x11/game_window_x11.cpp
        ${POMDOG_SRC_DIR}/platform/x11/game_window_x11.hpp
        ${POMDOG_SRC_DIR}/platform/x11/opengl_context_x11.cpp
        ${POMDOG_SRC_DIR}/platform/x11/opengl_context_x11.hpp
        ${POMDOG_SRC_DIR}/platform/x11/x11_atom_cache.hpp
        ${POMDOG_SRC_DIR}/platform/x11/x11_context.cpp
        ${POMDOG_SRC_DIR}/platform/x11/x11_context.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: platform/win32
        ${POMDOG_SRC_DIR}/platform/win32/bootstrap.cpp
        ${POMDOG_SRC_DIR}/platform/win32/bootstrap.hpp
        ${POMDOG_SRC_DIR}/platform/win32/bootstrap_settings_win32.hpp
        ${POMDOG_SRC_DIR}/platform/win32/dark_mode.cpp
        ${POMDOG_SRC_DIR}/platform/win32/dark_mode.hpp
        ${POMDOG_SRC_DIR}/platform/win32/game_host_win32.cpp
        ${POMDOG_SRC_DIR}/platform/win32/game_host_win32.hpp
        ${POMDOG_SRC_DIR}/platform/win32/game_window_win32.cpp
        ${POMDOG_SRC_DIR}/platform/win32/game_window_win32.hpp
        ${POMDOG_SRC_DIR}/platform/win32/prerequisites_win32.hpp
    >

    $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/win32
        ${POMDOG_SRC_DIR}/platform/win32/opengl_context_win32.cpp
        ${POMDOG_SRC_DIR}/platform/win32/opengl_context_win32.hpp
    >
)
