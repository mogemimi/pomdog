// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/Blend.hpp"
#include "Pomdog/Graphics/BlendOperation.hpp"
#include <utility>

namespace Pomdog {

struct POMDOG_EXPORT RenderTargetBlendDescription final {
    Blend ColorSourceBlend;
    Blend ColorDestinationBlend;
    BlendOperation ColorBlendOperation;
    Blend AlphaSourceBlend;
    Blend AlphaDestinationBlend;
    BlendOperation AlphaBlendOperation;
    bool BlendEnable;

    static RenderTargetBlendDescription CreateAdditive()
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = true;
        desc.ColorBlendOperation = BlendOperation::Add;
        desc.AlphaBlendOperation = BlendOperation::Add;
        desc.ColorSourceBlend = Blend::SourceAlpha;
        desc.AlphaSourceBlend = Blend::SourceAlpha;
        desc.ColorDestinationBlend = Blend::One;
        desc.AlphaDestinationBlend = Blend::One;
        return desc;
    }

    static RenderTargetBlendDescription CreateAlphaBlend()
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = true;
        desc.ColorBlendOperation = BlendOperation::Add;
        desc.AlphaBlendOperation = BlendOperation::Add;
        desc.ColorSourceBlend = Blend::One;
        desc.AlphaSourceBlend = Blend::One;
        desc.ColorDestinationBlend = Blend::InverseSourceAlpha;
        desc.AlphaDestinationBlend = Blend::InverseSourceAlpha;
        return desc;
    }

    static RenderTargetBlendDescription CreateNonPremultiplied()
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = true;
        desc.ColorBlendOperation = BlendOperation::Add;
        desc.AlphaBlendOperation = BlendOperation::Add;
        desc.ColorSourceBlend = Blend::SourceAlpha;
        desc.AlphaSourceBlend = Blend::SourceAlpha;
        desc.ColorDestinationBlend = Blend::InverseSourceAlpha;
        desc.AlphaDestinationBlend = Blend::InverseSourceAlpha;
        return desc;
    }

    static RenderTargetBlendDescription CreateOpaque()
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = false;
        desc.ColorBlendOperation = BlendOperation::Add;
        desc.AlphaBlendOperation = BlendOperation::Add;
        desc.ColorSourceBlend = Blend::One;
        desc.AlphaSourceBlend = Blend::One;
        desc.ColorDestinationBlend = Blend::Zero;
        desc.AlphaDestinationBlend = Blend::Zero;
        return desc;
    }
};

} // namespace Pomdog
