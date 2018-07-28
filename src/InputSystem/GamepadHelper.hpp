// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/PlayerIndex.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace Pomdog {

struct GamepadState;

namespace Detail {
namespace InputSystem {

struct GamepadDeviceID final {
    uint16_t BusType = 0;
    uint16_t Vendor = 0;
    uint16_t Product = 0;
    uint16_t Version = 0;
};

namespace GamepadHelper {

std::string ToString(const GamepadDeviceID& uuid);

int ToInt(PlayerIndex index);

void ClearState(GamepadState& state);

} // namespace GamepadHelper
} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
