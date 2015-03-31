// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BLEND_A27CB239_HPP
#define POMDOG_BLEND_A27CB239_HPP

#include <cstdint>

namespace Pomdog {

enum class Blend: std::uint8_t {
    Zero,
    One,
    SourceColor,
    InverseSourceColor,
    SourceAlpha,
    InverseSourceAlpha,
    DestinationAlpha,
    InverseDestinationAlpha,
    DestinationColor,
    InverseDestinationColor,
    SourceAlphaSaturation,
    BlendFactor,
    InvereseBlendFactor,
    Source1Color,
    InverseSource1Color,
    Source1Alpha,
    InverseSource1Alpha,
};

} // namespace Pomdog

#endif // POMDOG_BLEND_A27CB239_HPP
