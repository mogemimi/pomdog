// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/Blend.hpp"
#include "Pomdog/Graphics/BlendOperation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

struct POMDOG_EXPORT RenderTargetBlendDescription final {
    Blend ColorSourceBlend;
    Blend ColorDestinationBlend;
    BlendOperation ColorBlendOperation;
    Blend AlphaSourceBlend;
    Blend AlphaDestinationBlend;
    BlendOperation AlphaBlendOperation;
    bool BlendEnable;

    static RenderTargetBlendDescription CreateAdditive() noexcept
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

    static RenderTargetBlendDescription CreateAlphaBlend() noexcept
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

    static RenderTargetBlendDescription CreateNonPremultiplied() noexcept
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

    static RenderTargetBlendDescription CreateOpaque() noexcept
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
