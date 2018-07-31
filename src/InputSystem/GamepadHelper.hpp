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
namespace GamepadHelper {

int ToInt(PlayerIndex index);

void ClearState(GamepadState& state);

} // namespace GamepadHelper
} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
