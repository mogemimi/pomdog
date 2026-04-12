// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/gamepad_buttons.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class GamepadImpl;
} // namespace pomdog::detail

namespace pomdog::detail {

/// Identifies a logical gamepad button in the .gcdb binary format.
/// Values correspond to FlatBuffers int8 indices stored in the .gcdb binary.
/// NOTE: This ordering differs from GamepadButtons; use toGamepadButtons()
/// to convert.
enum class ButtonKind : i8 {
    Invalid = -1,
    A = 0,
    B = 1,
    X = 2,
    Y = 3,
    LeftShoulder = 4,
    RightShoulder = 5,
    LeftTrigger = 6,
    RightTrigger = 7,
    LeftMenu = 8,
    RightMenu = 9,
    LeftStick = 10,
    RightStick = 11,
    Guide = 12,
    Extra1 = 13,
    Extra2 = 14,
    DPadUp = 15,
    DPadDown = 16,
    DPadLeft = 17,
    DPadRight = 18,
};

/// Identifies a logical thumb-stick axis.
/// Values correspond to FlatBuffers int8 indices stored in the .gcdb binary.
enum class ThumbStickKind : i8 {
    Invalid = -1,
    LeftStickX = 0,
    LeftStickY = 1,
    RightStickX = 2,
    RightStickY = 3,
};

/// Describes how a single physical axis maps to a thumb-stick and/or
/// trigger pair. When `thumbStick` is valid the axis drives that stick;
/// when `positiveTrigger` / `negativeTrigger` are valid the positive or
/// negative half of the axis drives the corresponding trigger button.
struct AxisMapper final {
    ThumbStickKind thumbStick = ThumbStickKind::Invalid;
    ButtonKind positiveTrigger = ButtonKind::Invalid;
    ButtonKind negativeTrigger = ButtonKind::Invalid;
};

/// Maps 16 physical button indices to logical ButtonKind values.
using GamepadButtonMappings = std::array<ButtonKind, 16>;

/// A complete button/axis/hat mapping entry for a single gamepad device.
/// Populated from the FlatBuffers .gcdb binary or filled with
/// platform-specific defaults when no database entry is found.
struct GamepadMappingEntry final {
    GamepadButtonMappings buttons;
    std::array<AxisMapper, 6> axes;

    /// Maps hat directions to logical buttons.
    /// Index 0 = up, 1 = right, 2 = down, 3 = left.
    std::array<ButtonKind, 4> hats;
};

} // namespace pomdog::detail

namespace pomdog::detail::gamepad_mappings {

/// Converts a ButtonKind (internal .gcdb format) to the public
/// GamepadButtons enum.
[[nodiscard]] GamepadButtons
toGamepadButtons(ButtonKind kind) noexcept;

/// Applies a button state to the GamepadImpl for the physical button
/// at the given physical index according to the button mappings.
void applyButton(GamepadImpl& impl, const GamepadButtonMappings& mappings, int physicalIndex, bool isDown) noexcept;

/// Applies a button state to the GamepadImpl for the given ButtonKind.
void applyButton(GamepadImpl& impl, ButtonKind kind, bool isDown) noexcept;

/// Applies a thumb-stick axis value to the GamepadImpl.
void applyThumbStick(GamepadImpl& impl, ThumbStickKind kind, f32 value) noexcept;

/// Returns a pointer to the capability flag for the physical button at
/// `index`, or nullptr when the mapping is Invalid.
[[nodiscard]] unsafe_ptr<bool>
hasButton(GamepadCapabilities& caps, const GamepadButtonMappings& mappings, int index) noexcept;

/// Returns a pointer to the capability flag for the given logical `kind`,
/// or nullptr when `kind` is Invalid.
[[nodiscard]] unsafe_ptr<bool>
hasButton(GamepadCapabilities& caps, ButtonKind kind) noexcept;

/// Returns a pointer to the capability flag for the given thumb-stick
/// `kind`, or nullptr when `kind` is Invalid.
[[nodiscard]] unsafe_ptr<bool>
hasThumbStick(GamepadCapabilities& caps, ThumbStickKind kind) noexcept;

} // namespace pomdog::detail::gamepad_mappings
