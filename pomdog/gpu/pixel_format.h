// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// PixelFormat specifies the format used by pixels in the texture.
enum class PixelFormat : std::uint8_t {
    /// The pixel format is invalid. You cannot create a texture with this format.
    Invalid,

    /// A single-component, 8-bit unsigned-normalized integer format for alpha only.
    A8_UNorm,

    /// A single-component, 8-bit unsigned-normalized integer format that has 8-bits for the red channel.
    R8_UNorm,

    /// A two-component, 16-bit unsigned-normalized integer format that has 8-bits per channel.
    R8G8_UNorm,

    /// A four-component, 32-bit unsigned-normalized integer format that has 8-bits per channel.
    R8G8B8A8_UNorm,

    /// A four-component, 32-bit unsigned-normalized integer format that has
    /// 10-bits for each RGB color and a 2-bits for alpha.
    R10G10B10A2_UNorm,

    /// A three-component, 32-bit floating-point format that has
    /// a 11-bits for red and green channels, and a 10-bits for blue channel.
    R11G11B10_Float,

    /// A four-component, 32-bit unsigned-normalized integer format that has 8-bits per channel.
    B8G8R8A8_UNorm,

    /// A single-component, 32-bit floating-point format that has 32-bits for the red channel.
    R32_Float,

    /// A four-component, 128-bit floating-point format that has 32-bits per channel.
    R32G32B32A32_Float,

    /// A two-component, 32-bit floating-point format that has 16-bits per channel.
    R16G16_Float,

    /// A four-component, 64-bit floating-point format that has 16-bits per channel.
    R16G16B16A16_Float,

    /// A four-component, block-compression format for unsigned-normalized integer texel data.
    BlockComp1_UNorm,

    /// A four-component, block-compression format for unsigned-normalized integer texel data.
    BlockComp2_UNorm,

    /// A four-component, block-compression format for unsigned-normalized integer texel data.
    BlockComp3_UNorm,

    /// 16-bit buffer format. 16 bits for depth.
    Depth16,

    /// 32-bit buffer format. 24 bits for depth and 8 bits for stencil.
    ///
    /// The pixel format is only supported in certain devices for Metal, so you
    /// can use Depth32_Float_Stencil8_Uint instead.
    Depth24Stencil8,

    /// 32-bit floating-point buffer format. 32 bits for depth.
    Depth32,

    /// 32-bit floating-point buffer and 8 bit stencil buffer.
    Depth32_Float_Stencil8_Uint,
};

} // namespace pomdog
