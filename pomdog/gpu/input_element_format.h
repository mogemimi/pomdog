// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// InputElementFormat specifies the format of a vertex input element.
enum class InputElementFormat : u8 {
    /// A single-component, 32-bit floating-point format with 32-bits per channel.
    Float32x1 = 0,

    /// A two-component, 64-bit floating-point format with 32-bits per channel.
    Float32x2 = 1,

    /// A three-component, 96-bit floating-point format with 32-bits per channel.
    Float32x3 = 2,

    /// A four-component, 128-bit floating-point format with 32-bits per channel.
    Float32x4 = 3,

    /// A single-component, 8-bit unsigned integer format with 8-bits per channel.
    Uint8x1 = 4,

    /// A two-component, 16-bit unsigned integer format with 8-bits per channel.
    Uint8x2 = 5,

    /// A four-component, 32-bit unsigned integer format with 8-bits per channel.
    Uint8x4 = 6,

    /// A single-component, 8-bit unsigned normalized integer format with 8-bits per channel.
    Unorm8x1 = 7,

    /// A two-component, 16-bit unsigned normalized integer format with 8-bits per channel.
    Unorm8x2 = 8,

    /// A four-component, 32-bit unsigned normalized integer format with 8-bits per channel.
    Unorm8x4 = 9,

    /// A single-component, 32-bit integer format with 32-bits per channel.
    Int32x1 = 10,

    /// A two-component, 64-bit integer format with 32-bits per channel.
    Int32x2 = 11,

    /// A three-component, 96-bit integer format with 32-bits per channel.
    Int32x3 = 12,

    /// A four-component, 128-bit integer format with 32-bits per channel.
    Int32x4 = 13,

    /// A two-component, 32-bit floating-point format with 16-bits per channel.
    Float16x2 = 14,

    /// A four-component, 64-bit floating-point format with 16-bits per channel.
    Float16x4 = 15,
};

} // namespace pomdog::gpu
