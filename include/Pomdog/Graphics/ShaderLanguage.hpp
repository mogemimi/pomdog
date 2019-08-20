// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

/// ShaderLanguage specifies the shader language supported by the graphics device.
enum class ShaderLanguage : std::uint8_t {
    /// The OpenGL shading language (GLSL)
    GLSL,

    /// The DirectX high-level shading language (HLSL)
    HLSL,

    /// The Metal shading language
    Metal,
};

} // namespace Pomdog
