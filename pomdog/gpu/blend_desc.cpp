// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/blend_desc.h"

namespace pomdog::gpu {

BlendDesc BlendDesc::createDefault() noexcept
{
    return createOpaque();
}

BlendDesc BlendDesc::createAdditive() noexcept
{
    BlendDesc desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDesc::createAdditive();
    }
    return desc;
}

BlendDesc BlendDesc::createAlphaBlend() noexcept
{
    BlendDesc desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDesc::createAlphaBlend();
    }
    return desc;
}

BlendDesc BlendDesc::createNonPremultiplied() noexcept
{
    BlendDesc desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDesc::createNonPremultiplied();
    }
    return desc;
}

BlendDesc BlendDesc::createOpaque() noexcept
{
    BlendDesc desc;
    desc.alphaToCoverageEnable = false;
    desc.independentBlendEnable = false;
    for (auto& renderTarget : desc.renderTargets) {
        renderTarget = RenderTargetBlendDesc::createOpaque();
    }
    return desc;
}

} // namespace pomdog::gpu
