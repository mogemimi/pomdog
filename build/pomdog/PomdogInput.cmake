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
)
