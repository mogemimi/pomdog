// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/render_target_blend_descriptor.h"

namespace pomdog::gpu {

RenderTargetBlendDescriptor RenderTargetBlendDescriptor::CreateAdditive() noexcept
{
    RenderTargetBlendDescriptor desc;
    desc.BlendEnable = true;
    desc.ColorBlendOperation = BlendOperation::Add;
    desc.AlphaBlendOperation = BlendOperation::Add;
    desc.ColorSourceBlend = BlendFactor::SourceAlpha;
    desc.AlphaSourceBlend = BlendFactor::SourceAlpha;
    desc.ColorDestinationBlend = BlendFactor::One;
    desc.AlphaDestinationBlend = BlendFactor::One;
    return desc;
}

RenderTargetBlendDescriptor RenderTargetBlendDescriptor::CreateAlphaBlend() noexcept
{
    RenderTargetBlendDescriptor desc;
    desc.BlendEnable = true;
    desc.ColorBlendOperation = BlendOperation::Add;
    desc.AlphaBlendOperation = BlendOperation::Add;
    desc.ColorSourceBlend = BlendFactor::One;
    desc.AlphaSourceBlend = BlendFactor::One;
    desc.ColorDestinationBlend = BlendFactor::InverseSourceAlpha;
    desc.AlphaDestinationBlend = BlendFactor::InverseSourceAlpha;
    return desc;
}

RenderTargetBlendDescriptor RenderTargetBlendDescriptor::CreateNonPremultiplied() noexcept
{
    RenderTargetBlendDescriptor desc;
    desc.BlendEnable = true;
    desc.ColorBlendOperation = BlendOperation::Add;
    desc.AlphaBlendOperation = BlendOperation::Add;
    desc.ColorSourceBlend = BlendFactor::SourceAlpha;
    desc.AlphaSourceBlend = BlendFactor::SourceAlpha;
    desc.ColorDestinationBlend = BlendFactor::InverseSourceAlpha;
    desc.AlphaDestinationBlend = BlendFactor::InverseSourceAlpha;
    return desc;
}

RenderTargetBlendDescriptor RenderTargetBlendDescriptor::CreateOpaque() noexcept
{
    RenderTargetBlendDescriptor desc;
    desc.BlendEnable = false;
    desc.ColorBlendOperation = BlendOperation::Add;
    desc.AlphaBlendOperation = BlendOperation::Add;
    desc.ColorSourceBlend = BlendFactor::One;
    desc.AlphaSourceBlend = BlendFactor::One;
    desc.ColorDestinationBlend = BlendFactor::Zero;
    desc.AlphaDestinationBlend = BlendFactor::Zero;
    return desc;
}

} // namespace pomdog::gpu
