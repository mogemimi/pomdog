// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BLEND_A27CB239_A90C_46B1_841D_637B0D8343D6_HPP
#define POMDOG_BLEND_A27CB239_A90C_46B1_841D_637B0D8343D6_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

    //Source1Color,
    //InverseSource1Color,
    //Source1Alpha,
    //InverseSource1Alpha,
};

}// namespace Pomdog

#endif // !defined(POMDOG_BLEND_A27CB239_A90C_46B1_841D_637B0D8343D6_HPP)
