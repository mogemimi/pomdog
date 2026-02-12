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
    state.buttons.a = ButtonState::Up;
    state.buttons.b = ButtonState::Up;
    state.buttons.x = ButtonState::Up;
    state.buttons.y = ButtonState::Up;
    state.buttons.leftShoulder = ButtonState::Up;
    state.buttons.rightShoulder = ButtonState::Up;
    state.buttons.leftTrigger = ButtonState::Up;
    state.buttons.rightTrigger = ButtonState::Up;
    state.buttons.leftMenu = ButtonState::Up;
    state.buttons.rightMenu = ButtonState::Up;
    state.buttons.leftStick = ButtonState::Up;
    state.buttons.rightStick = ButtonState::Up;
    state.buttons.guide = ButtonState::Up;
    state.buttons.extra1 = ButtonState::Up;
    state.buttons.extra2 = ButtonState::Up;
    state.dpad.down = ButtonState::Up;
    state.dpad.up = ButtonState::Up;
    state.dpad.left = ButtonState::Up;
    state.dpad.right = ButtonState::Up;
    state.thumbSticks.left = Vector2::createZero();
    state.thumbSticks.right = Vector2::createZero();
    state.isConnected = false;
}

} // namespace pomdog::detail::GamepadHelper
