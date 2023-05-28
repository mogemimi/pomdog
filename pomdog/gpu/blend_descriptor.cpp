// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/blend_descriptor.h"

namespace pomdog::gpu {

BlendDescriptor BlendDescriptor::createDefault() noexcept
{
    return createOpaque();
}

BlendDescriptor BlendDescriptor::createAdditive() noexcept
{
    BlendDescriptor desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDescriptor::createAdditive();
    }
    return desc;
}

BlendDescriptor BlendDescriptor::createAlphaBlend() noexcept
{
    BlendDescriptor desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDescriptor::createAlphaBlend();
    }
    return desc;
}

BlendDescriptor BlendDescriptor::createNonPremultiplied() noexcept
{
    BlendDescriptor desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDescriptor::createNonPremultiplied();
    }
    return desc;
}

BlendDescriptor BlendDescriptor::createOpaque() noexcept
{
    BlendDescriptor desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDescriptor::createOpaque();
    }
    return desc;
}

} // namespace pomdog::gpu
