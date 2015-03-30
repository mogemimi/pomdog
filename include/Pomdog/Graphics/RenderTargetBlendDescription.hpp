// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RENDERTARGETBLENDDESCRIPTION_75AADD8A_HPP
#define POMDOG_RENDERTARGETBLENDDESCRIPTION_75AADD8A_HPP

#include "Blend.hpp"
#include "BlendFunction.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT RenderTargetBlendDescription {
public:
    Blend ColorSourceBlend;
    Blend ColorDestinationBlend;
    BlendFunction ColorBlendFunction;
    Blend AlphaSourceBlend;
    Blend AlphaDestinationBlend;
    BlendFunction AlphaBlendFunction;
    bool BlendEnable;

public:
    static RenderTargetBlendDescription CreateAdditive()
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = true;
        desc.ColorBlendFunction = BlendFunction::Add;
        desc.AlphaBlendFunction = BlendFunction::Add;
        desc.ColorSourceBlend = Blend::SourceAlpha;
        desc.AlphaSourceBlend = Blend::SourceAlpha;
        desc.ColorDestinationBlend = Blend::One;
        desc.AlphaDestinationBlend = Blend::One;
        return std::move(desc);
    }

    static RenderTargetBlendDescription CreateAlphaBlend()
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = true;
        desc.ColorBlendFunction = BlendFunction::Add;
        desc.AlphaBlendFunction = BlendFunction::Add;
        desc.ColorSourceBlend = Blend::One;
        desc.AlphaSourceBlend = Blend::One;
        desc.ColorDestinationBlend = Blend::InverseSourceAlpha;
        desc.AlphaDestinationBlend = Blend::InverseSourceAlpha;
        return std::move(desc);
    }

    static RenderTargetBlendDescription CreateNonPremultiplied()
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = true;
        desc.ColorBlendFunction = BlendFunction::Add;
        desc.AlphaBlendFunction = BlendFunction::Add;
        desc.ColorSourceBlend = Blend::SourceAlpha;
        desc.AlphaSourceBlend = Blend::SourceAlpha;
        desc.ColorDestinationBlend = Blend::InverseSourceAlpha;
        desc.AlphaDestinationBlend = Blend::InverseSourceAlpha;
        return std::move(desc);
    }

    static RenderTargetBlendDescription CreateOpaque()
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = false;
        desc.ColorBlendFunction = BlendFunction::Add;
        desc.AlphaBlendFunction = BlendFunction::Add;
        desc.ColorSourceBlend = Blend::One;
        desc.AlphaSourceBlend = Blend::One;
        desc.ColorDestinationBlend = Blend::Zero;
        desc.AlphaDestinationBlend = Blend::Zero;
        return std::move(desc);
    }
};

} // namespace Pomdog

#endif // POMDOG_RENDERTARGETBLENDDESCRIPTION_75AADD8A_HPP
