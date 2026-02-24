// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

enum class BlendOperation : u8 {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max,
};

} // namespace pomdog::gpu
