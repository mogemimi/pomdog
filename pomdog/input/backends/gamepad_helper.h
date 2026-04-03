// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {
enum class PlayerIndex : i8;
struct GamepadState;
} // namespace pomdog

namespace pomdog::detail::GamepadHelper {

[[nodiscard]] i32
toInt(PlayerIndex index) noexcept;

void clearState(GamepadState& state) noexcept;

} // namespace pomdog::detail::GamepadHelper
