source_group(input              REGULAR_EXPRESSION "pomdog/input/*")
source_group(input/backends     REGULAR_EXPRESSION "pomdog/input/backends/*")
source_group(input/cocoa        REGULAR_EXPRESSION "pomdog/input/cocoa/*")
source_group(input/directinput  REGULAR_EXPRESSION "pomdog/input/directinput/*")
source_group(input/emscripten   REGULAR_EXPRESSION "pomdog/input/emscripten/*")
source_group(input/iokit        REGULAR_EXPRESSION "pomdog/input/iokit/*")
source_group(input/linux        REGULAR_EXPRESSION "pomdog/input/linux/*")
source_group(input/win32        REGULAR_EXPRESSION "pomdog/input/win32/*")
source_group(input/x11          REGULAR_EXPRESSION "pomdog/input/x11/*")

target_sources(pomdog_static PRIVATE
    # NOTE: input
    ${POMDOG_SRC_DIR}/input/button_state.h
    ${POMDOG_SRC_DIR}/input/game_controller_db.cpp
    ${POMDOG_SRC_DIR}/input/game_controller_db.h
    ${POMDOG_SRC_DIR}/input/gamepad_buttons.h
    ${POMDOG_SRC_DIR}/input/gamepad_capabilities.h
    ${POMDOG_SRC_DIR}/input/gamepad_service.cpp
    ${POMDOG_SRC_DIR}/input/gamepad_service.h
    ${POMDOG_SRC_DIR}/input/gamepad_uuid.cpp
    ${POMDOG_SRC_DIR}/input/gamepad_uuid.h
    ${POMDOG_SRC_DIR}/input/gamepad.cpp
    ${POMDOG_SRC_DIR}/input/gamepad.h
    ${POMDOG_SRC_DIR}/input/key_state.h
    ${POMDOG_SRC_DIR}/input/keyboard.cpp
    ${POMDOG_SRC_DIR}/input/keyboard.h
    ${POMDOG_SRC_DIR}/input/keys.h
    ${POMDOG_SRC_DIR}/input/mouse_buttons.h
    ${POMDOG_SRC_DIR}/input/mouse.cpp
    ${POMDOG_SRC_DIR}/input/mouse.h
    ${POMDOG_SRC_DIR}/input/player_index.h
    ${POMDOG_SRC_DIR}/input/touchscreen.cpp
    ${POMDOG_SRC_DIR}/input/touchscreen.h

    # NOTE: input/backends
    ${POMDOG_SRC_DIR}/input/backends/gamepad_helper.cpp
    ${POMDOG_SRC_DIR}/input/backends/gamepad_helper.h
    ${POMDOG_SRC_DIR}/input/backends/gamepad_impl.cpp
    ${POMDOG_SRC_DIR}/input/backends/gamepad_impl.h
    ${POMDOG_SRC_DIR}/input/backends/gamepad_mapping_entry.cpp
    ${POMDOG_SRC_DIR}/input/backends/gamepad_mapping_entry.h
    ${POMDOG_SRC_DIR}/input/backends/keyboard_impl.cpp
    ${POMDOG_SRC_DIR}/input/backends/keyboard_impl.h
    ${POMDOG_SRC_DIR}/input/backends/mouse_impl.cpp
    ${POMDOG_SRC_DIR}/input/backends/mouse_impl.h

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: input/cocoa
        ${POMDOG_SRC_DIR}/input/cocoa/keyboard_cocoa.h
        ${POMDOG_SRC_DIR}/input/cocoa/keyboard_cocoa.cpp
        ${POMDOG_SRC_DIR}/input/cocoa/mouse_cocoa.h
        ${POMDOG_SRC_DIR}/input/cocoa/mouse_cocoa.cpp
    >

    $<$<PLATFORM_ID:Emscripten>:
        # NOTE: input/emscripten
        ${POMDOG_SRC_DIR}/input/emscripten/gamepad_emscripten.cpp
        ${POMDOG_SRC_DIR}/input/emscripten/gamepad_emscripten.h
        ${POMDOG_SRC_DIR}/input/emscripten/keyboard_emscripten.cpp
        ${POMDOG_SRC_DIR}/input/emscripten/keyboard_emscripten.h
        ${POMDOG_SRC_DIR}/input/emscripten/mouse_emscripten.cpp
        ${POMDOG_SRC_DIR}/input/emscripten/mouse_emscripten.h
        ${POMDOG_SRC_DIR}/input/emscripten/touchscreen_emscripten.cpp
        ${POMDOG_SRC_DIR}/input/emscripten/touchscreen_emscripten.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: input/directinput
        ${POMDOG_SRC_DIR}/input/directinput/gamepad_directinput.cpp
        ${POMDOG_SRC_DIR}/input/directinput/gamepad_directinput.h
        ${POMDOG_SRC_DIR}/input/directinput/prerequisites_directinput.h
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: input/iokit
        ${POMDOG_SRC_DIR}/input/iokit/gamepad_iokit.cpp
        ${POMDOG_SRC_DIR}/input/iokit/gamepad_iokit.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: input/linux
        ${POMDOG_SRC_DIR}/input/linux/gamepad_linux.cpp
        ${POMDOG_SRC_DIR}/input/linux/gamepad_linux.h
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: input/x11
        ${POMDOG_SRC_DIR}/input/x11/keyboard_x11.cpp
        ${POMDOG_SRC_DIR}/input/x11/keyboard_x11.h
        ${POMDOG_SRC_DIR}/input/x11/mouse_x11.cpp
        ${POMDOG_SRC_DIR}/input/x11/mouse_x11.h
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: input/win32
        ${POMDOG_SRC_DIR}/input/win32/keyboard_win32.cpp
        ${POMDOG_SRC_DIR}/input/win32/keyboard_win32.h
        ${POMDOG_SRC_DIR}/input/win32/mouse_win32.cpp
        ${POMDOG_SRC_DIR}/input/win32/mouse_win32.h
    >
)
