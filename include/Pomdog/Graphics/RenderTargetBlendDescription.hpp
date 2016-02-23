// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/Blend.hpp"
#include "Pomdog/Graphics/BlendOperation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <utility>

namespace Pomdog {

struct POMDOG_EXPORT RenderTargetBlendDescription {
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
        return std::move(desc);
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
        return std::move(desc);
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
        return std::move(desc);
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
        return std::move(desc);
    }
};

} // namespace Pomdog
