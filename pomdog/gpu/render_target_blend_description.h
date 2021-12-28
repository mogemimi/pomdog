// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/blend_factor.h"
#include "pomdog/gpu/blend_operation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct POMDOG_EXPORT RenderTargetBlendDescription final {
    BlendFactor ColorSourceBlend;
    BlendFactor ColorDestinationBlend;
    BlendOperation ColorBlendOperation;
    BlendFactor AlphaSourceBlend;
    BlendFactor AlphaDestinationBlend;
    BlendOperation AlphaBlendOperation;
    bool BlendEnable;

    static RenderTargetBlendDescription CreateAdditive() noexcept
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = true;
        desc.ColorBlendOperation = BlendOperation::Add;
        desc.AlphaBlendOperation = BlendOperation::Add;
        desc.ColorSourceBlend = BlendFactor::SourceAlpha;
        desc.AlphaSourceBlend = BlendFactor::SourceAlpha;
        desc.ColorDestinationBlend = BlendFactor::One;
        desc.AlphaDestinationBlend = BlendFactor::One;
        return desc;
    }

    static RenderTargetBlendDescription CreateAlphaBlend() noexcept
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = true;
        desc.ColorBlendOperation = BlendOperation::Add;
        desc.AlphaBlendOperation = BlendOperation::Add;
        desc.ColorSourceBlend = BlendFactor::One;
        desc.AlphaSourceBlend = BlendFactor::One;
        desc.ColorDestinationBlend = BlendFactor::InverseSourceAlpha;
        desc.AlphaDestinationBlend = BlendFactor::InverseSourceAlpha;
        return desc;
    }

    static RenderTargetBlendDescription CreateNonPremultiplied() noexcept
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = true;
        desc.ColorBlendOperation = BlendOperation::Add;
        desc.AlphaBlendOperation = BlendOperation::Add;
        desc.ColorSourceBlend = BlendFactor::SourceAlpha;
        desc.AlphaSourceBlend = BlendFactor::SourceAlpha;
        desc.ColorDestinationBlend = BlendFactor::InverseSourceAlpha;
        desc.AlphaDestinationBlend = BlendFactor::InverseSourceAlpha;
        return desc;
    }

    static RenderTargetBlendDescription CreateOpaque() noexcept
    {
        RenderTargetBlendDescription desc;
        desc.BlendEnable = false;
        desc.ColorBlendOperation = BlendOperation::Add;
        desc.AlphaBlendOperation = BlendOperation::Add;
        desc.ColorSourceBlend = BlendFactor::One;
        desc.AlphaSourceBlend = BlendFactor::One;
        desc.ColorDestinationBlend = BlendFactor::Zero;
        desc.AlphaDestinationBlend = BlendFactor::Zero;
        return desc;
    }
};

} // namespace pomdog
