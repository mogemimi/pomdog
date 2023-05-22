// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/rasterizer_descriptor.h"

namespace pomdog::gpu {

RasterizerDescriptor RasterizerDescriptor::CreateDefault() noexcept
{
    return CreateCullCounterClockwise();
}

RasterizerDescriptor RasterizerDescriptor::CreateCullClockwise() noexcept
{
    RasterizerDescriptor desc;
    desc.cullMode = CullMode::ClockwiseFace;
    desc.fillMode = FillMode::Solid;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

RasterizerDescriptor RasterizerDescriptor::CreateCullCounterClockwise() noexcept
{
    RasterizerDescriptor desc;
    desc.cullMode = CullMode::CounterClockwiseFace;
    desc.fillMode = FillMode::Solid;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

RasterizerDescriptor RasterizerDescriptor::CreateCullNone() noexcept
{
    RasterizerDescriptor desc;
    desc.cullMode = CullMode::None;
    desc.fillMode = FillMode::Solid;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

RasterizerDescriptor RasterizerDescriptor::CreateCullNoneWireframe() noexcept
{
    RasterizerDescriptor desc;
    desc.cullMode = CullMode::None;
    desc.fillMode = FillMode::Wireframe;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

} // namespace pomdog::gpu
