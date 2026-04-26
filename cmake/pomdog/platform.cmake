source_group(platform/cocoa       REGULAR_EXPRESSION "pomdog/platform/cocoa/*")
source_group(platform/emscripten  REGULAR_EXPRESSION "pomdog/platform/emscripten/*")
source_group(platform/win32       REGULAR_EXPRESSION "pomdog/platform/win32/*")
source_group(platform/x11         REGULAR_EXPRESSION "pomdog/platform/x11/*")

target_sources(pomdog_static PRIVATE
    $<$<PLATFORM_ID:Darwin>:
        # NOTE: platform/cocoa
        ${POMDOG_SRC_DIR}/platform/cocoa/bootstrap_cocoa.h
        ${POMDOG_SRC_DIR}/platform/cocoa/bootstrap_cocoa.mm
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: platform/emscripten
        ${POMDOG_SRC_DIR}/platform/emscripten/bootstrap_emscripten.cpp
        ${POMDOG_SRC_DIR}/platform/emscripten/bootstrap_emscripten.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: platform/x11
        ${POMDOG_SRC_DIR}/platform/x11/bootstrap_x11.h
        ${POMDOG_SRC_DIR}/platform/x11/bootstrap_x11.cpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: platform/win32
        ${POMDOG_SRC_DIR}/platform/win32/bootstrap_win32.cpp
        ${POMDOG_SRC_DIR}/platform/win32/bootstrap_win32.h
    >
)
