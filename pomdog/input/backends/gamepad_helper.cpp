// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/gamepad_helper.h"
#include "pomdog/input/player_index.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail::GamepadHelper {

[[nodiscard]] i32
toInt(PlayerIndex index) noexcept
{
    static_assert(static_cast<i32>(PlayerIndex::One) == 1);
    POMDOG_ASSERT(static_cast<i32>(index) >= 1);
    return static_cast<i32>(index) - 1;
}

} // namespace pomdog::detail::GamepadHelper
