// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class ShaderLanguage : std::uint8_t {
    ///@brief The OpenGL shading language (GLSL)
    GLSL,

    ///@brief The DirectX high-level shading language (HLSL)
    HLSL,

    ///@brief The Metal shading language
    Metal,
};

} // namespace Pomdog
