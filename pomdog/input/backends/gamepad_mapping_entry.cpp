// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/gamepad_mapping_entry.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/gamepad_state.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::gamepad_mappings {
namespace {

[[nodiscard]] ButtonKind
toButtonIndex(int physicalIndex, GamepadButtonMappings mappings)
{
    if ((physicalIndex < 0) || (physicalIndex >= static_cast<int>(mappings.size()))) {
        return ButtonKind::Invalid;
    }
    POMDOG_ASSERT(physicalIndex >= 0);
    POMDOG_ASSERT(physicalIndex < static_cast<int>(mappings.size()));
    return mappings[physicalIndex];
}

} // namespace

[[nodiscard]] unsafe_ptr<ButtonState>
getButton(GamepadState& state, const GamepadButtonMappings& mappings, int physicalIndex) noexcept
{
    const auto kind = toButtonIndex(physicalIndex, mappings);
    return getButton(state, kind);
}

[[nodiscard]] unsafe_ptr<ButtonState>
getButton(GamepadState& state, ButtonKind kind) noexcept
{
    if (kind == ButtonKind::Invalid) {
        return nullptr;
    }
    const auto index = static_cast<int>(kind);

    std::array<ButtonState*, 19> buttons = {{
        &state.buttons.a,
        &state.buttons.b,
        &state.buttons.x,
        &state.buttons.y,
        &state.buttons.leftShoulder,
        &state.buttons.rightShoulder,
        &state.buttons.leftTrigger,
        &state.buttons.rightTrigger,
        &state.buttons.leftMenu,
        &state.buttons.rightMenu,
        &state.buttons.leftStick,
        &state.buttons.rightStick,
        &state.buttons.guide,
        &state.buttons.extra1,
        &state.buttons.extra2,
        &state.dpad.up,
        &state.dpad.down,
        &state.dpad.left,
        &state.dpad.right,
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(buttons.size()));
    return buttons[index];
}

[[nodiscard]] unsafe_ptr<f32>
getThumbStick(GamepadState& state, ThumbStickKind kind) noexcept
{
    if (kind == ThumbStickKind::Invalid) {
        return nullptr;
    }
    const auto index = static_cast<int>(kind);

    std::array<unsafe_ptr<f32>, 4> axes = {{
        &state.thumbSticks.left.x,
        &state.thumbSticks.left.y,
        &state.thumbSticks.right.x,
        &state.thumbSticks.right.y,
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(axes.size()));
    return axes[index];
}

[[nodiscard]] unsafe_ptr<bool>
hasButton(GamepadCapabilities& caps, const GamepadButtonMappings& mappings, int physicalIndex) noexcept
{
    const auto kind = toButtonIndex(physicalIndex, mappings);
    return hasButton(caps, kind);
}

[[nodiscard]] unsafe_ptr<bool>
hasButton(GamepadCapabilities& caps, ButtonKind kind) noexcept
{
    if (kind == ButtonKind::Invalid) {
        return nullptr;
    }
    const auto index = static_cast<int>(kind);

    std::array<unsafe_ptr<bool>, 19> buttons = {{
        &caps.hasAButton,
        &caps.hasBButton,
        &caps.hasXButton,
        &caps.hasYButton,
        &caps.hasLeftShoulderButton,
        &caps.hasRightShoulderButton,
        &caps.hasLeftTrigger,
        &caps.hasRightTrigger,
        &caps.hasLeftMenuButton,
        &caps.hasRightMenuButton,
        &caps.hasLeftStickButton,
        &caps.hasRightStickButton,
        &caps.hasGuideButton,
        &caps.hasExtra1Button,
        &caps.hasExtra2Button,
        nullptr, // DPad.Up
        nullptr, // DPad.Down
        nullptr, // DPad.Left
        nullptr, // DPad.Right
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(buttons.size()));
    return buttons[index];
}

[[nodiscard]] unsafe_ptr<bool>
hasThumbStick(GamepadCapabilities& caps, ThumbStickKind kind) noexcept
{
    if (kind == ThumbStickKind::Invalid) {
        return nullptr;
    }
    const auto index = static_cast<int>(kind);

    std::array<unsafe_ptr<bool>, 4> axes = {{
        &caps.hasLeftXThumbStick,
        &caps.hasLeftYThumbStick,
        &caps.hasRightXThumbStick,
        &caps.hasRightYThumbStick,
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(axes.size()));
    return axes[index];
}

} // namespace pomdog::detail::gamepad_mappings
