// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/render_target_blend_desc.h"

namespace pomdog::gpu {

RenderTargetBlendDesc RenderTargetBlendDesc::createAdditive() noexcept
{
    RenderTargetBlendDesc desc = {};
    desc.blendEnable = true;
    desc.colorBlendOperation = BlendOperation::Add;
    desc.alphaBlendOperation = BlendOperation::Add;
    desc.colorSourceBlend = BlendFactor::SrcAlpha;
    desc.alphaSourceBlend = BlendFactor::SrcAlpha;
    desc.colorDestinationBlend = BlendFactor::One;
    desc.alphaDestinationBlend = BlendFactor::One;
    return desc;
}

RenderTargetBlendDesc RenderTargetBlendDesc::createAlphaBlend() noexcept
{
    RenderTargetBlendDesc desc = {};
    desc.blendEnable = true;
    desc.colorBlendOperation = BlendOperation::Add;
    desc.alphaBlendOperation = BlendOperation::Add;
    desc.colorSourceBlend = BlendFactor::One;
    desc.alphaSourceBlend = BlendFactor::One;
    desc.colorDestinationBlend = BlendFactor::OneMinusSrcAlpha;
    desc.alphaDestinationBlend = BlendFactor::OneMinusSrcAlpha;
    return desc;
}

RenderTargetBlendDesc RenderTargetBlendDesc::createNonPremultiplied() noexcept
{
    RenderTargetBlendDesc desc = {};
    desc.blendEnable = true;
    desc.colorBlendOperation = BlendOperation::Add;
    desc.alphaBlendOperation = BlendOperation::Add;
    desc.colorSourceBlend = BlendFactor::SrcAlpha;
    desc.alphaSourceBlend = BlendFactor::SrcAlpha;
    desc.colorDestinationBlend = BlendFactor::OneMinusSrcAlpha;
    desc.alphaDestinationBlend = BlendFactor::OneMinusSrcAlpha;
    return desc;
}

RenderTargetBlendDesc RenderTargetBlendDesc::createOpaque() noexcept
{
    RenderTargetBlendDesc desc = {};
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
