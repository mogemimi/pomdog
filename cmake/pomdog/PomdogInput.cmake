target_sources(pomdog_static PRIVATE
    # NOTE: Input
    ${POMDOG_INC_DIR}/Input/ButtonState.hpp
    ${POMDOG_INC_DIR}/Input/Gamepad.hpp
    ${POMDOG_INC_DIR}/Input/GamepadButtons.hpp
    ${POMDOG_INC_DIR}/Input/GamepadCapabilities.hpp
    ${POMDOG_INC_DIR}/Input/GamepadDPad.hpp
    ${POMDOG_INC_DIR}/Input/GamepadState.hpp
    ${POMDOG_INC_DIR}/Input/GamepadThumbSticks.hpp
    ${POMDOG_INC_DIR}/Input/GamepadUUID.hpp
    ${POMDOG_SRC_DIR}/Input/GamepadUUID.cpp
    ${POMDOG_INC_DIR}/Input/Keyboard.hpp
    ${POMDOG_INC_DIR}/Input/KeyboardState.hpp
    ${POMDOG_SRC_DIR}/Input/KeyboardState.cpp
    ${POMDOG_INC_DIR}/Input/Keys.hpp
    ${POMDOG_INC_DIR}/Input/KeyState.hpp
    ${POMDOG_INC_DIR}/Input/Mouse.hpp
    ${POMDOG_INC_DIR}/Input/MouseState.hpp
    ${POMDOG_INC_DIR}/Input/PlayerIndex.hpp
    ${POMDOG_INC_DIR}/Input/TouchLocation.hpp
    ${POMDOG_INC_DIR}/Input/TouchLocationState.hpp

    # NOTE: Input.Backends
    ${POMDOG_SRC_DIR}/Input.Backends/GamepadHelper.cpp
    ${POMDOG_SRC_DIR}/Input.Backends/GamepadHelper.hpp
    ${POMDOG_SRC_DIR}/Input.Backends/GamepadMappings.cpp
    ${POMDOG_SRC_DIR}/Input.Backends/GamepadMappings.hpp
    ${POMDOG_SRC_DIR}/Input.Backends/NativeGamepad.hpp

    $<$<PLATFORM_ID:Windows>:
        # NOTE: Input.DirectInput
        ${POMDOG_SRC_DIR}/Input.DirectInput/GamepadDirectInput.cpp
        ${POMDOG_SRC_DIR}/Input.DirectInput/GamepadDirectInput.hpp
        ${POMDOG_SRC_DIR}/Input.DirectInput/PrerequisitesDirectInput.hpp
    >

    $<$<PLATFORM_ID:Darwin>:
        # NOTE: Input.IOKit
        ${POMDOG_SRC_DIR}/Input.IOKit/GamepadIOKit.cpp
        ${POMDOG_SRC_DIR}/Input.IOKit/GamepadIOKit.hpp
    >

    $<$<PLATFORM_ID:Linux>:
        # NOTE: Input.Linux
        ${POMDOG_SRC_DIR}/Input.Linux/GamepadLinux.cpp
        ${POMDOG_SRC_DIR}/Input.Linux/GamepadLinux.hpp
    >
)
