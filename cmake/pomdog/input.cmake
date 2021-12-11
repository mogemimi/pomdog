source_group(input              REGULAR_EXPRESSION "pomdog/input/*")
source_group(input/backends     REGULAR_EXPRESSION "pomdog/input/backends/*")
source_group(input/cocoa        REGULAR_EXPRESSION "pomdog/input/cocoa/*")
source_group(input/directinput  REGULAR_EXPRESSION "pomdog/input/directinput/*")
source_group(input/iokit        REGULAR_EXPRESSION "pomdog/input/iokit/*")
source_group(input/linux        REGULAR_EXPRESSION "pomdog/input/linux/*")
source_group(input/win32        REGULAR_EXPRESSION "pomdog/input/win32/*")
source_group(input/x11          REGULAR_EXPRESSION "pomdog/input/x11/*")

target_sources(pomdog_static PRIVATE
    # NOTE: input
    ${POMDOG_SRC_DIR}/input/button_state.hpp
    ${POMDOG_SRC_DIR}/input/gamepad_buttons.hpp
    ${POMDOG_SRC_DIR}/input/gamepad_capabilities.hpp
    ${POMDOG_SRC_DIR}/input/gamepad_dpad.hpp
    ${POMDOG_SRC_DIR}/input/gamepad_state.hpp
    ${POMDOG_SRC_DIR}/input/gamepad_thumbsticks.hpp
    ${POMDOG_SRC_DIR}/input/gamepad_uuid.cpp
    ${POMDOG_SRC_DIR}/input/gamepad_uuid.hpp
    ${POMDOG_SRC_DIR}/input/gamepad.cpp
    ${POMDOG_SRC_DIR}/input/gamepad.hpp
    ${POMDOG_SRC_DIR}/input/key_state.hpp
    ${POMDOG_SRC_DIR}/input/keyboard_state.cpp
    ${POMDOG_SRC_DIR}/input/keyboard_state.hpp
    ${POMDOG_SRC_DIR}/input/keyboard.cpp
    ${POMDOG_SRC_DIR}/input/keyboard.hpp
    ${POMDOG_SRC_DIR}/input/keys.hpp
    ${POMDOG_SRC_DIR}/input/mouse_buttons.hpp
    ${POMDOG_SRC_DIR}/input/mouse_state.hpp
    ${POMDOG_SRC_DIR}/input/mouse.cpp
    ${POMDOG_SRC_DIR}/input/mouse.hpp
    ${POMDOG_SRC_DIR}/input/player_index.hpp
    ${POMDOG_SRC_DIR}/input/touch_location_state.hpp
    ${POMDOG_SRC_DIR}/input/touch_location.hpp

    # NOTE: input/backends
    ${POMDOG_SRC_DIR}/input/backends/gamepad_helper.cpp
    ${POMDOG_SRC_DIR}/input/backends/gamepad_helper.hpp
    ${POMDOG_SRC_DIR}/input/backends/gamepad_mappings.cpp
    ${POMDOG_SRC_DIR}/input/backends/gamepad_mappings.hpp
    ${POMDOG_SRC_DIR}/input/backends/native_gamepad.cpp
    ${POMDOG_SRC_DIR}/input/backends/native_gamepad.hpp

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: input/cocoa
        ${POMDOG_SRC_DIR}/input/cocoa/keyboard_cocoa.hpp
        ${POMDOG_SRC_DIR}/input/cocoa/keyboard_cocoa.cpp
        ${POMDOG_SRC_DIR}/input/cocoa/mouse_cocoa.hpp
        ${POMDOG_SRC_DIR}/input/cocoa/mouse_cocoa.cpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: input/directinput
        ${POMDOG_SRC_DIR}/input/directinput/gamepad_directinput.cpp
        ${POMDOG_SRC_DIR}/input/directinput/gamepad_directinput.hpp
        ${POMDOG_SRC_DIR}/input/directinput/prerequisites_directinput.hpp
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: input/iokit
        ${POMDOG_SRC_DIR}/input/iokit/gamepad_iokit.cpp
        ${POMDOG_SRC_DIR}/input/iokit/gamepad_iokit.hpp
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: input/linux
        ${POMDOG_SRC_DIR}/input/linux/gamepad_linux.cpp
        ${POMDOG_SRC_DIR}/input/linux/gamepad_linux.hpp
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: input/x11
        ${POMDOG_SRC_DIR}/input/x11/keyboard_x11.cpp
        ${POMDOG_SRC_DIR}/input/x11/keyboard_x11.hpp
        ${POMDOG_SRC_DIR}/input/x11/mouse_x11.cpp
        ${POMDOG_SRC_DIR}/input/x11/mouse_x11.hpp
    >

    $<$<PLATFORM_ID:Windows>:
        # NOTE: input/win32
        ${POMDOG_SRC_DIR}/input/win32/keyboard_win32.cpp
        ${POMDOG_SRC_DIR}/input/win32/keyboard_win32.hpp
        ${POMDOG_SRC_DIR}/input/win32/mouse_win32.cpp
        ${POMDOG_SRC_DIR}/input/win32/mouse_win32.hpp
    >
)
