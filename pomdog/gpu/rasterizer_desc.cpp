// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/rasterizer_desc.h"

namespace pomdog::gpu {

RasterizerDesc RasterizerDesc::createDefault() noexcept
{
    return createCullCounterClockwise();
}

RasterizerDesc RasterizerDesc::createCullClockwise() noexcept
{
    RasterizerDesc desc = {};
    desc.cullMode = CullMode::ClockwiseFace;
    desc.fillMode = FillMode::Solid;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

RasterizerDesc RasterizerDesc::createCullCounterClockwise() noexcept
{
    RasterizerDesc desc = {};
    desc.cullMode = CullMode::CounterClockwiseFace;
    desc.fillMode = FillMode::Solid;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

RasterizerDesc RasterizerDesc::createCullNone() noexcept
{
    RasterizerDesc desc = {};
    desc.cullMode = CullMode::None;
    desc.fillMode = FillMode::Solid;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

RasterizerDesc RasterizerDesc::createCullNoneWireframe() noexcept
{
    RasterizerDesc desc = {};
    desc.cullMode = CullMode::None;
    desc.fillMode = FillMode::Wireframe;
    desc.depthBias = 0;
    desc.slopeScaledDepthBias = 0;
    desc.multisampleEnable = false;
    return desc;
}

} // namespace pomdog::gpu
