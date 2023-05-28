// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/depth_stencil_descriptor.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

DepthStencilDescriptor DepthStencilDescriptor::createDefault() noexcept
{
    return createReadWriteDepth();
}

DepthStencilDescriptor DepthStencilDescriptor::createReadWriteDepth() noexcept
{
    DepthStencilDescriptor desc;
    desc.depthBufferEnable = true;
    desc.depthBufferWriteEnable = true;
    desc.stencilEnable = false;
    desc.depthBufferFunction = ComparisonFunction::LessEqual;
    desc.referenceStencil = 0;
    desc.stencilMask = std::numeric_limits<std::uint32_t>::max();
    desc.stencilWriteMask = std::numeric_limits<std::uint32_t>::max();
    DepthStencilOperation defaultOperation = {
        StencilOperation::Keep,
        StencilOperation::Keep,
        StencilOperation::Keep,
        ComparisonFunction::Always,
    };
    desc.clockwiseFace = defaultOperation;
    desc.counterClockwiseFace = defaultOperation;
    return desc;
}

DepthStencilDescriptor DepthStencilDescriptor::createReadOnlyDepth() noexcept
{
    DepthStencilDescriptor desc;
    desc.depthBufferEnable = true;
    desc.depthBufferWriteEnable = false;
    desc.stencilEnable = false;
    desc.depthBufferFunction = ComparisonFunction::LessEqual;
    desc.referenceStencil = 0;
    desc.stencilMask = std::numeric_limits<std::uint32_t>::max();
    desc.stencilWriteMask = std::numeric_limits<std::uint32_t>::max();
    DepthStencilOperation defaultOperation = {
        StencilOperation::Keep,
        StencilOperation::Keep,
        StencilOperation::Keep,
        ComparisonFunction::Always,
    };
    desc.clockwiseFace = defaultOperation;
    desc.counterClockwiseFace = defaultOperation;
    return desc;
}

DepthStencilDescriptor DepthStencilDescriptor::createNone() noexcept
{
    DepthStencilDescriptor desc;
    desc.depthBufferEnable = false;
    desc.depthBufferWriteEnable = false;
    desc.stencilEnable = false;
    desc.depthBufferFunction = ComparisonFunction::Always;
    desc.referenceStencil = 0;
    desc.stencilMask = std::numeric_limits<std::uint32_t>::max();
    desc.stencilWriteMask = std::numeric_limits<std::uint32_t>::max();
    DepthStencilOperation defaultOperation = {
        StencilOperation::Keep,
        StencilOperation::Keep,
        StencilOperation::Keep,
        ComparisonFunction::Always,
    };
    desc.clockwiseFace = defaultOperation;
    desc.counterClockwiseFace = defaultOperation;
    return desc;
}

} // namespace pomdog::gpu
