// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// ShaderLanguage specifies the shader language supported by the graphics device.
enum class ShaderLanguage : std::uint8_t {
    /// The OpenGL shading language (GLSL)
    GLSL,

    /// The DirectX high-level shading language (HLSL)
    HLSL,

    /// The Metal shading language
    Metal,
};

} // namespace pomdog::gpu
