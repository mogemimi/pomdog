source_group(platform/cocoa      REGULAR_EXPRESSION "pomdog/platform/cocoa/*")
source_group(platform/win32      REGULAR_EXPRESSION "pomdog/platform/win32/*")
source_group(platform/x11        REGULAR_EXPRESSION "pomdog/platform/x11/*")

target_sources(pomdog_static PRIVATE
    $<$<PLATFORM_ID:Darwin>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/platform/cocoa/bootstrap_cocoa.hpp
        ${POMDOG_SRC_DIR}/platform/cocoa/bootstrap_cocoa.mm
    >

    $<$<AND:$<PLATFORM_ID:Darwin>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/platform/cocoa/opengl_context_cocoa.hpp
        ${POMDOG_SRC_DIR}/platform/cocoa/opengl_context_cocoa.mm
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: platform/x11
        ${POMDOG_SRC_DIR}/platform/x11/bootstrap_x11.hpp
        ${POMDOG_SRC_DIR}/platform/x11/bootstrap_x11.cpp
        ${POMDOG_SRC_DIR}/platform/x11/opengl_context_x11.cpp
        ${POMDOG_SRC_DIR}/platform/x11/opengl_context_x11.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: platform/win32
        ${POMDOG_SRC_DIR}/platform/win32/bootstrap_win32.cpp
        ${POMDOG_SRC_DIR}/platform/win32/bootstrap_win32.hpp
        ${POMDOG_SRC_DIR}/platform/win32/bootstrap_settings_win32.hpp
        ${POMDOG_SRC_DIR}/platform/win32/prerequisites_win32.hpp
    >

    $<$<AND:$<PLATFORM_ID:Windows>,$<BOOL:${POMDOG_USE_GL4}>>:
        # NOTE: platform/win32
        ${POMDOG_SRC_DIR}/platform/win32/opengl_context_win32.cpp
        ${POMDOG_SRC_DIR}/platform/win32/opengl_context_win32.hpp
    >
)
