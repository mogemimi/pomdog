// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/gamepad_helper.hpp"
#include "pomdog/input/gamepad_state.hpp"
#include "pomdog/utility/assert.hpp"

namespace pomdog::detail::GamepadHelper {

[[nodiscard]] int ToInt(PlayerIndex index) noexcept
{
    POMDOG_ASSERT(static_cast<int>(index) >= 1);
    return static_cast<int>(index) - 1;
}

void ClearState(GamepadState& state) noexcept
{
    state.Buttons.A = ButtonState::Released;
    state.Buttons.B = ButtonState::Released;
    state.Buttons.X = ButtonState::Released;
    state.Buttons.Y = ButtonState::Released;
    state.Buttons.LeftShoulder = ButtonState::Released;
    state.Buttons.RightShoulder = ButtonState::Released;
    state.Buttons.LeftTrigger = ButtonState::Released;
    state.Buttons.RightTrigger = ButtonState::Released;
    state.Buttons.LeftMenu = ButtonState::Released;
    state.Buttons.RightMenu = ButtonState::Released;
    state.Buttons.LeftStick = ButtonState::Released;
    state.Buttons.RightStick = ButtonState::Released;
    state.Buttons.Guide = ButtonState::Released;
    state.Buttons.Extra1 = ButtonState::Released;
    state.Buttons.Extra2 = ButtonState::Released;
    state.DPad.Down = ButtonState::Released;
    state.DPad.Up = ButtonState::Released;
    state.DPad.Left = ButtonState::Released;
    state.DPad.Right = ButtonState::Released;
    state.ThumbSticks.Left = Vector2::Zero();
    state.ThumbSticks.Right = Vector2::Zero();
    state.IsConnected = false;
}

} // namespace pomdog::detail::GamepadHelper
