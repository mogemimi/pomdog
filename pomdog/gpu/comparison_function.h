// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// ComparisonFunction specifies the comparison operator for depth and stencil tests.
enum class ComparisonFunction : u8 {
    /// The test always fails.
    Never,

    /// Passes if the incoming value is less than the stored value.
    Less,

    /// Passes if the incoming value equals the stored value.
    Equal,

    /// Passes if the incoming value is less than or equal to the stored value.
    LessEqual,

    /// Passes if the incoming value is greater than the stored value.
    Greater,

    /// Passes if the incoming value is not equal to the stored value.
    NotEqual,

    /// Passes if the incoming value is greater than or equal to the stored value.
    GreaterEqual,

    /// The test always passes.
    Always,
};

} // namespace pomdog::gpu
