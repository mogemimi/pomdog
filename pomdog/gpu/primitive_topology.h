// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

enum class PrimitiveTopology : u8 {
    /// A triangle list.
    TriangleList,

    /// A triangle strip.
    TriangleStrip,

    /// A line list.
    LineList,

    /// A line strip.
    LineStrip,
};

} // namespace pomdog::gpu
