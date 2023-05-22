// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/render_target_blend_descriptor.h"

namespace pomdog::gpu {

RenderTargetBlendDescriptor RenderTargetBlendDescriptor::CreateAdditive() noexcept
{
    RenderTargetBlendDescriptor desc;
    desc.blendEnable = true;
    desc.colorBlendOperation = BlendOperation::Add;
    desc.alphaBlendOperation = BlendOperation::Add;
    desc.colorSourceBlend = BlendFactor::SourceAlpha;
    desc.alphaSourceBlend = BlendFactor::SourceAlpha;
    desc.colorDestinationBlend = BlendFactor::One;
    desc.alphaDestinationBlend = BlendFactor::One;
    return desc;
}

RenderTargetBlendDescriptor RenderTargetBlendDescriptor::CreateAlphaBlend() noexcept
{
    RenderTargetBlendDescriptor desc;
    desc.blendEnable = true;
    desc.colorBlendOperation = BlendOperation::Add;
    desc.alphaBlendOperation = BlendOperation::Add;
    desc.colorSourceBlend = BlendFactor::One;
    desc.alphaSourceBlend = BlendFactor::One;
    desc.colorDestinationBlend = BlendFactor::InverseSourceAlpha;
    desc.alphaDestinationBlend = BlendFactor::InverseSourceAlpha;
    return desc;
}

RenderTargetBlendDescriptor RenderTargetBlendDescriptor::CreateNonPremultiplied() noexcept
{
    RenderTargetBlendDescriptor desc;
    desc.blendEnable = true;
    desc.colorBlendOperation = BlendOperation::Add;
    desc.alphaBlendOperation = BlendOperation::Add;
    desc.colorSourceBlend = BlendFactor::SourceAlpha;
    desc.alphaSourceBlend = BlendFactor::SourceAlpha;
    desc.colorDestinationBlend = BlendFactor::InverseSourceAlpha;
    desc.alphaDestinationBlend = BlendFactor::InverseSourceAlpha;
    return desc;
}

RenderTargetBlendDescriptor RenderTargetBlendDescriptor::CreateOpaque() noexcept
{
    RenderTargetBlendDescriptor desc;
    desc.blendEnable = false;
    desc.colorBlendOperation = BlendOperation::Add;
    desc.alphaBlendOperation = BlendOperation::Add;
    desc.colorSourceBlend = BlendFactor::One;
    desc.alphaSourceBlend = BlendFactor::One;
    desc.colorDestinationBlend = BlendFactor::Zero;
    desc.alphaDestinationBlend = BlendFactor::Zero;
    return desc;
}

} // namespace pomdog::gpu
