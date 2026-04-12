// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {
enum class PlayerIndex : i8;
} // namespace pomdog

namespace pomdog::detail::GamepadHelper {

[[nodiscard]] i32
toInt(PlayerIndex index) noexcept;

} // namespace pomdog::detail::GamepadHelper
