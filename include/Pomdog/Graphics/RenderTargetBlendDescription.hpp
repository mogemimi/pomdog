// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Blend.hpp"
#include "BlendFunction.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <utility>

namespace Pomdog {

struct POMDOG_EXPORT RenderTargetBlendDescription {
    Blend ColorSourceBlend;
    Blend ColorDestinationBlend;
    BlendFunction ColorBlendFunction;
    Blend AlphaSourceBlend;
    Blend AlphaDestinationBlend;
    BlendFunction AlphaBlendFunction;
    bool BlendEnable;

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
