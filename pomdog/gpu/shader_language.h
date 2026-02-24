// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// ShaderLanguage specifies the shader language supported by the graphics device.
enum class ShaderLanguage : u8 {
    /// The OpenGL shading language (GLSL)
    GLSL,

    /// The DirectX high-level shading language (HLSL)
    HLSL,

    /// The Metal shading language
    Metal,
};

} // namespace pomdog::gpu
