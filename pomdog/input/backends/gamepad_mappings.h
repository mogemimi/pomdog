// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/gamepad_helper.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/gamepad_state.h"
#include "pomdog/input/gamepad_uuid.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

enum class ButtonKind : std::int8_t {
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

enum class ThumbStickKind : std::int8_t {
    Invalid = -1,
    LeftStickX = 0,
    LeftStickY = 1,
    RightStickX = 2,
    RightStickY = 3,
};

struct AxisMapper final {
    ThumbStickKind thumbStick = ThumbStickKind::Invalid;
    ButtonKind positiveTrigger = ButtonKind::Invalid;
    ButtonKind negativeTrigger = ButtonKind::Invalid;
};

using GamepadButtonMappings = std::array<ButtonKind, 16>;

struct GamepadMappings final {
    GamepadButtonMappings buttons;
    std::array<AxisMapper, 6> axes;
};

[[nodiscard]] ButtonState*
GetButton(GamepadState& state, const GamepadButtonMappings& mappings, int index) noexcept;

[[nodiscard]] ButtonState*
GetButton(GamepadState& state, ButtonKind kind) noexcept;

[[nodiscard]] float*
GetThumbStick(GamepadState& state, ThumbStickKind kind) noexcept;

[[nodiscard]] bool*
HasButton(GamepadCapabilities& caps, const GamepadButtonMappings& mappings, int index) noexcept;

[[nodiscard]] bool*
HasButton(GamepadCapabilities& caps, ButtonKind kind) noexcept;

[[nodiscard]] bool*
HasThumbStick(GamepadCapabilities& caps, ThumbStickKind kind) noexcept;

[[nodiscard]] std::tuple<GamepadMappings, std::string>
GetMappings(const GamepadUUID& uuid) noexcept;

[[nodiscard]] std::tuple<GamepadMappings, std::string>
GetMappings(const std::string& uuid) noexcept;

} // namespace pomdog::detail
