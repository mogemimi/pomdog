// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// IndexFormat specifies the format of index buffer indices.
enum class IndexFormat : u8 {
    /// Specifies that indices are 16-bit unsigned integer values.
    UInt16,

    /// Specifies that indices are 32-bit unsigned integer values.
    UInt32,
};

} // namespace pomdog::gpu
