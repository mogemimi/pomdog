// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {

/// Represents the pressed or released state of a keyboard key.
enum class KeyState : u8 {
    Up = 0,
    Down = 1,
};

} // namespace pomdog
