// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/gamepad_helper.h"
#include "pomdog/input/gamepad_state.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail::GamepadHelper {

[[nodiscard]] int toInt(PlayerIndex index) noexcept
{
    POMDOG_ASSERT(static_cast<int>(index) >= 1);
    return static_cast<int>(index) - 1;
}

void clearState(GamepadState& state) noexcept
{
    state.buttons.a = ButtonState::Released;
    state.buttons.b = ButtonState::Released;
    state.buttons.x = ButtonState::Released;
    state.buttons.y = ButtonState::Released;
    state.buttons.leftShoulder = ButtonState::Released;
    state.buttons.rightShoulder = ButtonState::Released;
    state.buttons.leftTrigger = ButtonState::Released;
    state.buttons.rightTrigger = ButtonState::Released;
    state.buttons.leftMenu = ButtonState::Released;
    state.buttons.rightMenu = ButtonState::Released;
    state.buttons.leftStick = ButtonState::Released;
    state.buttons.rightStick = ButtonState::Released;
    state.buttons.guide = ButtonState::Released;
    state.buttons.extra1 = ButtonState::Released;
    state.buttons.extra2 = ButtonState::Released;
    state.dpad.down = ButtonState::Released;
    state.dpad.up = ButtonState::Released;
    state.dpad.left = ButtonState::Released;
    state.dpad.right = ButtonState::Released;
    state.thumbSticks.left = Vector2::createZero();
    state.thumbSticks.right = Vector2::createZero();
    state.isConnected = false;
}

} // namespace pomdog::detail::GamepadHelper
