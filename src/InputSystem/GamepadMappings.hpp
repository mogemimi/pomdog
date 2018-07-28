// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "GamepadHelper.hpp"
#include "Pomdog/Input/GamepadCapabilities.hpp"
#include "Pomdog/Input/GamepadState.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

namespace Pomdog {
namespace Detail {
namespace InputSystem {

enum class AxesKind : std::int8_t {
    None = -1,
    LeftStickX = 0,
    LeftStickY = 1,
    RightStickX = 2,
    RightStickY = 3,
    LeftTrigger = 4,
    RightTrigger = 5,
};

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
};

using GamepadButtonMappings = std::array<ButtonKind, 16>;

struct GamepadMappings {
    GamepadButtonMappings buttons;
    std::array<AxesKind, 6> axes;
};

ButtonState* GetButton(GamepadState& state, const GamepadButtonMappings& mappings, int index);

std::tuple<GamepadMappings, std::string> GetMappings(const GamepadDeviceID& uuid);

} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
