// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "GamepadHelper.hpp"
#include "Pomdog/Input/GamepadCapabilities.hpp"
#include "Pomdog/Input/GamepadState.hpp"
#include "Pomdog/Input/GamepadUUID.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

namespace Pomdog {
namespace Detail {
namespace InputSystem {

enum class ButtonKind : std::int8_t {
    None = -1,
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
    None = -1,
    LeftStickX = 0,
    LeftStickY = 1,
    RightStickX = 2,
    RightStickY = 3,
};

struct AxisMapper final {
    ThumbStickKind thumbStick = ThumbStickKind::None;
    ButtonKind positiveTrigger = ButtonKind::None;
    ButtonKind negativeTrigger = ButtonKind::None;
};

using GamepadButtonMappings = std::array<ButtonKind, 16>;

struct GamepadMappings final {
    GamepadButtonMappings buttons;
    std::array<AxisMapper, 6> axes;
};

ButtonState* GetButton(GamepadState& state, const GamepadButtonMappings& mappings, int index);
ButtonState* GetButton(GamepadState& state, ButtonKind kind);

float* GetThumbStick(GamepadState& state, ThumbStickKind kind);

bool* HasButton(GamepadCapabilities& caps, const GamepadButtonMappings& mappings, int index);
bool* HasButton(GamepadCapabilities& caps, ButtonKind kind);

bool* HasThumbStick(GamepadCapabilities& caps, ThumbStickKind kind);

std::tuple<GamepadMappings, std::string> GetMappings(const GamepadUUID& uuid);

std::tuple<GamepadMappings, std::string> GetMappings(const std::string& uuid);

} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
