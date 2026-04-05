// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/depth_stencil_desc.h"

namespace pomdog::gpu {

DepthStencilDesc DepthStencilDesc::createDefault() noexcept
{
    return createReadWriteDepth();
}

DepthStencilDesc DepthStencilDesc::createReadWriteDepth() noexcept
{
    DepthStencilDesc desc = {};
    desc.depthBufferEnable = true;
    desc.depthBufferWriteEnable = true;
    desc.stencilEnable = false;
    desc.depthBufferFunction = ComparisonFunction::LessEqual;
    desc.referenceStencil = 0;
    desc.stencilMask = std::numeric_limits<u32>::max();
    desc.stencilWriteMask = std::numeric_limits<u32>::max();
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

DepthStencilDesc DepthStencilDesc::createReadOnlyDepth() noexcept
{
    DepthStencilDesc desc = {};
    desc.depthBufferEnable = true;
    desc.depthBufferWriteEnable = false;
    desc.stencilEnable = false;
    desc.depthBufferFunction = ComparisonFunction::LessEqual;
    desc.referenceStencil = 0;
    desc.stencilMask = std::numeric_limits<u32>::max();
    desc.stencilWriteMask = std::numeric_limits<u32>::max();
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

DepthStencilDesc DepthStencilDesc::createNone() noexcept
{
    DepthStencilDesc desc = {};
    desc.depthBufferEnable = false;
    desc.depthBufferWriteEnable = false;
    desc.stencilEnable = false;
    desc.depthBufferFunction = ComparisonFunction::Always;
    desc.referenceStencil = 0;
    desc.stencilMask = std::numeric_limits<u32>::max();
    desc.stencilWriteMask = std::numeric_limits<u32>::max();
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
