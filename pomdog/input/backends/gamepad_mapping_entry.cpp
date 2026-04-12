// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/gamepad_mapping_entry.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/gamepad_impl.h"
#include "pomdog/input/button_state.h"
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

[[nodiscard]] GamepadButtons
toGamepadButtons(ButtonKind kind) noexcept
{
    // NOTE: ButtonKind and GamepadButtons have different orderings for
    // Guide/Extra1/Extra2/DPad values due to .gcdb binary compatibility.
    switch (kind) {
    case ButtonKind::A:
        return GamepadButtons::A;
    case ButtonKind::B:
        return GamepadButtons::B;
    case ButtonKind::X:
        return GamepadButtons::X;
    case ButtonKind::Y:
        return GamepadButtons::Y;
    case ButtonKind::LeftShoulder:
        return GamepadButtons::LeftBumper;
    case ButtonKind::RightShoulder:
        return GamepadButtons::RightBumper;
    case ButtonKind::LeftTrigger:
        return GamepadButtons::LeftTrigger;
    case ButtonKind::RightTrigger:
        return GamepadButtons::RightTrigger;
    case ButtonKind::LeftMenu:
        return GamepadButtons::LeftMenu;
    case ButtonKind::RightMenu:
        return GamepadButtons::RightMenu;
    case ButtonKind::LeftStick:
        return GamepadButtons::LeftStick;
    case ButtonKind::RightStick:
        return GamepadButtons::RightStick;
    case ButtonKind::Guide:
        return GamepadButtons::Guide;
    case ButtonKind::Extra1:
        return GamepadButtons::Extra1;
    case ButtonKind::Extra2:
        return GamepadButtons::Extra2;
    case ButtonKind::DPadUp:
        return GamepadButtons::DPadUp;
    case ButtonKind::DPadDown:
        return GamepadButtons::DPadDown;
    case ButtonKind::DPadLeft:
        return GamepadButtons::DPadLeft;
    case ButtonKind::DPadRight:
        return GamepadButtons::DPadRight;
    case ButtonKind::Invalid:
        return GamepadButtons::Invalid;
    }
    return GamepadButtons::Invalid;
}

void applyButton(GamepadImpl& impl, const GamepadButtonMappings& mappings, int physicalIndex, bool isDown) noexcept
{
    const auto kind = toButtonIndex(physicalIndex, mappings);
    applyButton(impl, kind, isDown);
}

void applyButton(GamepadImpl& impl, ButtonKind kind, bool isDown) noexcept
{
    const auto button = toGamepadButtons(kind);
    if (button == GamepadButtons::Invalid) {
        return;
    }
    const auto buttonState = isDown ? ButtonState::Down : ButtonState::Up;
    const f32 analogValue = isDown ? 1.0f : 0.0f;
    impl.setButtonState(button, buttonState, analogValue);
}

void applyThumbStick(GamepadImpl& impl, ThumbStickKind kind, f32 value) noexcept
{
    switch (kind) {
    case ThumbStickKind::LeftStickX:
        impl.setLeftStickX(value);
        break;
    case ThumbStickKind::LeftStickY:
        impl.setLeftStickY(value);
        break;
    case ThumbStickKind::RightStickX:
        impl.setRightStickX(value);
        break;
    case ThumbStickKind::RightStickY:
        impl.setRightStickY(value);
        break;
    case ThumbStickKind::Invalid:
        break;
    }
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
