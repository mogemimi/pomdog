// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// BlendOperation specifies the arithmetic operation applied to the source and destination color values.
enum class BlendOperation : u8 {
    /// Computes `result = (src * srcFactor) + (dst * dstFactor)`.
    Add,

    /// Computes `result = (src * srcFactor) - (dst * dstFactor)`.
    Subtract,

    /// Computes `result = (dst * dstFactor) - (src * srcFactor)`.
    ReverseSubtract,

    /// Selects `result = min(src, dst)`; blend factors are ignored.
    Min,

    /// Selects `result = max(src, dst)`; blend factors are ignored.
    Max,
};

} // namespace pomdog::gpu
