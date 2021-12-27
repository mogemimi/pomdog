// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/player_index.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
struct GamepadState;
} // namespace pomdog

namespace pomdog::detail::GamepadHelper {

[[nodiscard]] int ToInt(PlayerIndex index) noexcept;

void ClearState(GamepadState& state) noexcept;

} // namespace pomdog::detail::GamepadHelper
