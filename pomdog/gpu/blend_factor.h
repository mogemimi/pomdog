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
    SourceColor,

    /// One minus the RGB components of the source color.
    InverseSourceColor,

    /// Alpha component of the source color.
    SourceAlpha,

    /// One minus the alpha component of the source color.
    InverseSourceAlpha,

    /// Alpha component of the destination color, typically the current framebuffer value.
    DestinationAlpha,

    /// One minus the alpha component of the destination color.
    InverseDestinationAlpha,

    /// RGB components of the destination color, typically the current framebuffer value.
    DestinationColor,

    /// One minus the RGB components of the destination color.
    InverseDestinationColor,

    /// Uses `min(srcAlpha, 1 - dstAlpha)`; applies only to color components.
    SourceAlphaSaturation,

    /// Constant blend color set via `CommandList::setBlendFactor()`.
    /// RGB blending uses the RGB components; alpha blending uses the alpha component.
    BlendFactor,

    /// One minus the constant blend color set via `CommandList::setBlendFactor()`.
    /// RGB blending uses `1 - RGB`; alpha blending uses `1 - alpha`.
    InverseBlendFactor,

    /// RGB components of the second shader output; requires dual-source blending.
    /// Not supported by the Emscripten/WebGL backend; falls back to `One`.
    Source1Color,

    /// One minus the RGB components of the second shader output; requires dual-source blending.
    /// Not supported by the Emscripten/WebGL backend; falls back to `Zero`.
    InverseSource1Color,

    /// Alpha component of the second shader output; requires dual-source blending.
    /// Not supported by the Emscripten/WebGL backend; falls back to `One`.
    Source1Alpha,

    /// One minus the alpha component of the second shader output; requires dual-source blending.
    /// Not supported by the Emscripten/WebGL backend; falls back to `Zero`.
    InverseSource1Alpha,
};

} // namespace pomdog::gpu
