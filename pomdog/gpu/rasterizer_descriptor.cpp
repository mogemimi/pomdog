// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/rasterizer_descriptor.h"

namespace pomdog::gpu {

RasterizerDescriptor RasterizerDescriptor::createDefault() noexcept
{
    return createCullCounterClockwise();
}

RasterizerDescriptor RasterizerDescriptor::createCullClockwise() noexcept
{
    RasterizerDescriptor desc;
    desc.cullMode = CullMode::ClockwiseFace;
    desc.fillMode = FillMode::Solid;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

RasterizerDescriptor RasterizerDescriptor::createCullCounterClockwise() noexcept
{
    RasterizerDescriptor desc;
    desc.cullMode = CullMode::CounterClockwiseFace;
    desc.fillMode = FillMode::Solid;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

RasterizerDescriptor RasterizerDescriptor::createCullNone() noexcept
{
    RasterizerDescriptor desc;
    desc.cullMode = CullMode::None;
    desc.fillMode = FillMode::Solid;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

RasterizerDescriptor RasterizerDescriptor::createCullNoneWireframe() noexcept
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
