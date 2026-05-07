// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// BlendFactor specifies the source or destination factor in a blend equation.
enum class BlendFactor : u8 {
    /// A blend factor of 0; the input does not contribute to the result.
    Zero,

    /// A blend factor of 1; the input value is used unchanged.
    One,

    /// RGB components of the source color, typically the pixel shader output.
    SrcColor,

    /// One minus the RGB components of the source color.
    OneMinusSrcColor,

    /// Alpha component of the source color.
    SrcAlpha,

    /// One minus the alpha component of the source color.
    OneMinusSrcAlpha,

    /// Alpha component of the destination color, typically the current framebuffer value.
    DstAlpha,

    /// One minus the alpha component of the destination color.
    OneMinusDstAlpha,

    /// RGB components of the destination color, typically the current framebuffer value.
    DstColor,

    /// One minus the RGB components of the destination color.
    OneMinusDstColor,

    /// Uses `min(srcAlpha, 1 - dstAlpha)`; applies only to color components.
    SrcAlphaSaturated,

    /// Constant blend color set via `CommandList::setBlendFactor()`.
    /// RGB blending uses the RGB components; alpha blending uses the alpha component.
    Constant,

    /// One minus the constant blend color set via `CommandList::setBlendFactor()`.
    /// RGB blending uses `1 - RGB`; alpha blending uses `1 - alpha`.
    OneMinusConstant,

    /// RGB components of the second shader output; requires dual-source blending.
    /// Not supported by the Emscripten/WebGL backend; falls back to `One`.
    Src1Color,

    /// One minus the RGB components of the second shader output; requires dual-source blending.
    /// Not supported by the Emscripten/WebGL backend; falls back to `Zero`.
    OneMinusSrc1Color,

    /// Alpha component of the second shader output; requires dual-source blending.
    /// Not supported by the Emscripten/WebGL backend; falls back to `One`.
    Src1Alpha,

    /// One minus the alpha component of the second shader output; requires dual-source blending.
    /// Not supported by the Emscripten/WebGL backend; falls back to `Zero`.
    OneMinusSrc1Alpha,
};

} // namespace pomdog::gpu
