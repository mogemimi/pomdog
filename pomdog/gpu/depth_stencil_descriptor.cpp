// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/depth_stencil_descriptor.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

DepthStencilDescriptor DepthStencilDescriptor::CreateDefault() noexcept
{
    return CreateReadWriteDepth();
}

DepthStencilDescriptor DepthStencilDescriptor::CreateReadWriteDepth() noexcept
{
    DepthStencilDescriptor desc;
    desc.DepthBufferEnable = true;
    desc.DepthBufferWriteEnable = true;
    desc.StencilEnable = false;
    desc.DepthBufferFunction = ComparisonFunction::LessEqual;
    desc.ReferenceStencil = 0;
    desc.StencilMask = std::numeric_limits<std::uint32_t>::max();
    desc.StencilWriteMask = std::numeric_limits<std::uint32_t>::max();
    DepthStencilOperation defaultOperation = {
        StencilOperation::Keep,
        StencilOperation::Keep,
        StencilOperation::Keep,
        ComparisonFunction::Always,
    };
    desc.ClockwiseFace = defaultOperation;
    desc.CounterClockwiseFace = defaultOperation;
    return desc;
}

DepthStencilDescriptor DepthStencilDescriptor::CreateReadOnlyDepth() noexcept
{
    DepthStencilDescriptor desc;
    desc.DepthBufferEnable = true;
    desc.DepthBufferWriteEnable = false;
    desc.StencilEnable = false;
    desc.DepthBufferFunction = ComparisonFunction::LessEqual;
    desc.ReferenceStencil = 0;
    desc.StencilMask = std::numeric_limits<std::uint32_t>::max();
    desc.StencilWriteMask = std::numeric_limits<std::uint32_t>::max();
    DepthStencilOperation defaultOperation = {
        StencilOperation::Keep,
        StencilOperation::Keep,
        StencilOperation::Keep,
        ComparisonFunction::Always,
    };
    desc.ClockwiseFace = defaultOperation;
    desc.CounterClockwiseFace = defaultOperation;
    return desc;
}

DepthStencilDescriptor DepthStencilDescriptor::CreateNone() noexcept
{
    DepthStencilDescriptor desc;
    desc.DepthBufferEnable = false;
    desc.DepthBufferWriteEnable = false;
    desc.StencilEnable = false;
    desc.DepthBufferFunction = ComparisonFunction::Always;
    desc.ReferenceStencil = 0;
    desc.StencilMask = std::numeric_limits<std::uint32_t>::max();
    desc.StencilWriteMask = std::numeric_limits<std::uint32_t>::max();
    DepthStencilOperation defaultOperation = {
        StencilOperation::Keep,
        StencilOperation::Keep,
        StencilOperation::Keep,
        ComparisonFunction::Always,
    };
    desc.ClockwiseFace = defaultOperation;
    desc.CounterClockwiseFace = defaultOperation;
    return desc;
}

} // namespace pomdog::gpu
