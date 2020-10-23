// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/PlayerIndex.hpp"
#include <cstdint>

namespace Pomdog {
struct GamepadState;
} // namespace Pomdog

namespace Pomdog::Detail::GamepadHelper {

[[nodiscard]] int ToInt(PlayerIndex index) noexcept;

void ClearState(GamepadState& state) noexcept;

} // namespace Pomdog::Detail::GamepadHelper
