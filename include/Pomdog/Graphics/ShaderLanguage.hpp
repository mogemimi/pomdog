// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERLANGUAGE_D009E979_HPP
#define POMDOG_SHADERLANGUAGE_D009E979_HPP

#include <cstdint>

namespace Pomdog {

enum class ShaderLanguage: std::uint8_t {
    ///@brief The OpenGL shading language (GLSL)
    GLSL,

    ///@brief The DirectX high-level shading language (HLSL)
    HLSL,

    ///@brief The Metal shading language
    Metal,
};

}// namespace Pomdog

#endif // POMDOG_SHADERLANGUAGE_D009E979_HPP
