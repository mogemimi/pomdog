// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Input/PlayerIndex.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
struct GamepadState;
} // namespace Pomdog

namespace Pomdog::Detail::GamepadHelper {

[[nodiscard]] int ToInt(PlayerIndex index) noexcept;

void ClearState(GamepadState& state) noexcept;

} // namespace Pomdog::Detail::GamepadHelper
