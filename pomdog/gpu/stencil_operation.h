// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// StencilOperation specifies the action to take on a stored stencil value when a test passes or fails.
enum class StencilOperation : u8 {
    /// Keeps the current stencil value unchanged.
    Keep,

    /// Sets the stencil value to 0.
    Zero,

    /// Replaces the stencil value with the reference value.
    Replace,

    /// Increments the stencil value, clamping at the maximum representable value.
    IncrementSaturation,

    /// Decrements the stencil value, clamping at 0.
    DecrementSaturation,

    /// Bitwise-inverts the stencil value.
    Invert,

    /// Increments the stencil value, wrapping around on overflow.
    Increment,

    /// Decrements the stencil value, wrapping around on underflow.
    Decrement,
};

} // namespace pomdog::gpu
