// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

namespace pomdog {

enum class TouchLocationState : u8 {
    Invalid,
    Moved,
    Pressed,
    Released,
};

} // namespace pomdog
