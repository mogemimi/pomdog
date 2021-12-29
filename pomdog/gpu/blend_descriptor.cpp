// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/blend_descriptor.h"

namespace pomdog::gpu {

BlendDescriptor BlendDescriptor::CreateDefault() noexcept
{
    return CreateOpaque();
}

BlendDescriptor BlendDescriptor::CreateAdditive() noexcept
{
    BlendDescriptor desc;
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;
    for (auto& renderTarget : desc.RenderTargets) {
        renderTarget = RenderTargetBlendDescriptor::CreateAdditive();
    }
    return desc;
}

BlendDescriptor BlendDescriptor::CreateAlphaBlend() noexcept
{
    BlendDescriptor desc;
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;
    for (auto& renderTarget : desc.RenderTargets) {
        renderTarget = RenderTargetBlendDescriptor::CreateAlphaBlend();
    }
    return desc;
}

BlendDescriptor BlendDescriptor::CreateNonPremultiplied() noexcept
{
    BlendDescriptor desc;
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;
    for (auto& renderTarget : desc.RenderTargets) {
        renderTarget = RenderTargetBlendDescriptor::CreateNonPremultiplied();
    }
    return desc;
}

BlendDescriptor BlendDescriptor::CreateOpaque() noexcept
{
    BlendDescriptor desc;
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;
    for (auto& renderTarget : desc.RenderTargets) {
        renderTarget = RenderTargetBlendDescriptor::CreateOpaque();
    }
    return desc;
}

} // namespace pomdog::gpu
