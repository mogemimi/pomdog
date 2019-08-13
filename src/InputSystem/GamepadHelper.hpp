// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/PlayerIndex.hpp"
#include <cstdint>
#include <memory>
#include <string>

namespace Pomdog {
struct GamepadState;
} // namespace Pomdog

namespace Pomdog::Detail::InputSystem::GamepadHelper {

int ToInt(PlayerIndex index);

void ClearState(GamepadState& state);

} // namespace Pomdog::Detail::InputSystem::GamepadHelper
