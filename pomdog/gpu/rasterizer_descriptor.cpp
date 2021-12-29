// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/rasterizer_descriptor.h"

namespace pomdog {

RasterizerDescriptor RasterizerDescriptor::CreateDefault() noexcept
{
    return CreateCullCounterClockwise();
}

RasterizerDescriptor RasterizerDescriptor::CreateCullClockwise() noexcept
{
    RasterizerDescriptor desc;
    desc.CullMode = CullMode::ClockwiseFace;
    desc.FillMode = FillMode::Solid;
    desc.DepthBias = 0;
    desc.SlopeScaledDepthBias = 0;
    desc.MultisampleEnable = false;
    return desc;
}

RasterizerDescriptor RasterizerDescriptor::CreateCullCounterClockwise() noexcept
{
    RasterizerDescriptor desc;
    desc.CullMode = CullMode::CounterClockwiseFace;
    desc.FillMode = FillMode::Solid;
    desc.DepthBias = 0;
    desc.SlopeScaledDepthBias = 0;
    desc.MultisampleEnable = false;
    return desc;
}

RasterizerDescriptor RasterizerDescriptor::CreateCullNone() noexcept
{
    RasterizerDescriptor desc;
    desc.CullMode = CullMode::None;
    desc.FillMode = FillMode::Solid;
    desc.DepthBias = 0;
    desc.SlopeScaledDepthBias = 0;
    desc.MultisampleEnable = false;
    return desc;
}

} // namespace pomdog
