// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BLENDDESCRIPTION_0C296679_HPP
#define POMDOG_BLENDDESCRIPTION_0C296679_HPP

#include "Blend.hpp"
#include "BlendFunction.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <limits>

namespace Pomdog {

class POMDOG_EXPORT BlendDescription {
public:
    Color BlendFactor = Color::White;

    std::uint32_t MultiSampleMask = std::numeric_limits<std::uint32_t>::max();

    BlendFunction ColorBlendFunction = BlendFunction::Add;

    BlendFunction AlphaBlendFunction = BlendFunction::Add;

    Blend ColorSourceBlend = Blend::One;

    Blend AlphaSourceBlend = Blend::One;

    Blend ColorDestinationBlend = Blend::One;

    Blend AlphaDestinationBlend = Blend::One;

    bool AlphaToCoverageEnable = false;

public:
    static BlendDescription CreateAdditive()
    {
        BlendDescription desc;
        desc.BlendFactor = Color::White;
        desc.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
        desc.AlphaToCoverageEnable = false;

        desc.ColorBlendFunction = BlendFunction::Add;
        desc.AlphaBlendFunction = BlendFunction::Add;
        desc.ColorSourceBlend = Blend::SourceAlpha;
        desc.AlphaSourceBlend = Blend::SourceAlpha;
        desc.ColorDestinationBlend = Blend::One;
        desc.AlphaDestinationBlend = Blend::One;
        return desc;
    }

    static BlendDescription CreateAlphaBlend()
    {
        BlendDescription desc;
        desc.BlendFactor = Color::White;
        desc.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
        desc.AlphaToCoverageEnable = false;

        desc.ColorBlendFunction = BlendFunction::Add;
        desc.AlphaBlendFunction = BlendFunction::Add;
        desc.ColorSourceBlend = Blend::One;
        desc.AlphaSourceBlend = Blend::One;
        desc.ColorDestinationBlend = Blend::InverseSourceAlpha;
        desc.AlphaDestinationBlend = Blend::InverseSourceAlpha;
        return desc;
    }

    static BlendDescription CreateNonPremultiplied()
    {
        BlendDescription desc;
        desc.BlendFactor = Color::White;
        desc.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
        desc.AlphaToCoverageEnable = false;

        desc.ColorBlendFunction = BlendFunction::Add;
        desc.AlphaBlendFunction = BlendFunction::Add;
        desc.ColorSourceBlend = Blend::SourceAlpha;
        desc.AlphaSourceBlend = Blend::SourceAlpha;
        desc.ColorDestinationBlend = Blend::InverseSourceAlpha;
        desc.AlphaDestinationBlend = Blend::InverseSourceAlpha;
        return desc;
    }

    static BlendDescription CreateOpaque()
    {
        BlendDescription desc;
        desc.BlendFactor = Color::White;
        desc.MultiSampleMask = std::numeric_limits<std::uint32_t>::max();
        desc.AlphaToCoverageEnable = false;

        desc.ColorBlendFunction = BlendFunction::Add;
        desc.AlphaBlendFunction = BlendFunction::Add;
        desc.ColorSourceBlend = Blend::One;
        desc.AlphaSourceBlend = Blend::One;
        desc.ColorDestinationBlend = Blend::Zero;
        desc.AlphaDestinationBlend = Blend::Zero;
        return desc;
    }
};

}// namespace Pomdog

#endif // POMDOG_BLENDDESCRIPTION_0C296679_HPP
