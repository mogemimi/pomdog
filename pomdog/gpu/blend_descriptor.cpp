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
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDescriptor::CreateAdditive();
    }
    return desc;
}

BlendDescriptor BlendDescriptor::CreateAlphaBlend() noexcept
{
    BlendDescriptor desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDescriptor::CreateAlphaBlend();
    }
    return desc;
}

BlendDescriptor BlendDescriptor::CreateNonPremultiplied() noexcept
{
    BlendDescriptor desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDescriptor::CreateNonPremultiplied();
    }
    return desc;
}

BlendDescriptor BlendDescriptor::CreateOpaque() noexcept
{
    BlendDescriptor desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDescriptor::CreateOpaque();
    }
    return desc;
}

} // namespace pomdog::gpu
