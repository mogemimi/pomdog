// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_DEPTHSTENCILDESCRIPTION_E14C5D77_HPP
#define POMDOG_DEPTHSTENCILDESCRIPTION_E14C5D77_HPP

#include "ComparisonFunction.hpp"
#include "DepthStencilOperation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <limits>

namespace Pomdog {

struct POMDOG_EXPORT DepthStencilDescription {
    DepthStencilOperation ClockwiseFace;
    DepthStencilOperation CounterClockwiseFace;
    std::int32_t ReferenceStencil;
    std::uint32_t StencilMask;
    std::uint32_t StencilWriteMask;
    ComparisonFunction DepthBufferFunction;
    bool DepthBufferEnable;
    bool DepthBufferWriteEnable;
    bool StencilEnable;

    static DepthStencilDescription CreateDefault()
    {
        return CreateReadWriteDepth();
    }

    static DepthStencilDescription CreateReadWriteDepth()
    {
        DepthStencilDescription desc;
        desc.DepthBufferEnable = true;
        desc.DepthBufferWriteEnable = true;
        desc.StencilEnable = false;
        desc.DepthBufferFunction = ComparisonFunction::LessEqual;
        desc.ReferenceStencil = 0;
        desc.StencilMask = std::numeric_limits<std::uint32_t>::max();
        desc.StencilWriteMask = std::numeric_limits<std::uint32_t>::max();
        DepthStencilOperation defaultOperation = {
            StencilOperation::Keep, StencilOperation::Keep,
            StencilOperation::Keep, ComparisonFunction::Always
        };
        desc.ClockwiseFace = defaultOperation;
        desc.CounterClockwiseFace = defaultOperation;
        return desc;
    }

    static DepthStencilDescription CreateReadOnlyDepth()
    {
        DepthStencilDescription desc;
        desc.DepthBufferEnable = true;
        desc.DepthBufferWriteEnable = false;
        desc.StencilEnable = false;
        desc.DepthBufferFunction = ComparisonFunction::LessEqual;
        desc.ReferenceStencil = 0;
        desc.StencilMask = std::numeric_limits<std::uint32_t>::max();
        desc.StencilWriteMask = std::numeric_limits<std::uint32_t>::max();
        DepthStencilOperation defaultOperation = {
            StencilOperation::Keep, StencilOperation::Keep,
            StencilOperation::Keep, ComparisonFunction::Always
        };
        desc.ClockwiseFace = defaultOperation;
        desc.CounterClockwiseFace = defaultOperation;
        return desc;
    }

    static DepthStencilDescription CreateNone()
    {
        DepthStencilDescription desc;
        desc.DepthBufferEnable = false;
        desc.DepthBufferWriteEnable = false;
        desc.StencilEnable = false;
        desc.DepthBufferFunction = ComparisonFunction::LessEqual;
        desc.ReferenceStencil = 0;
        desc.StencilMask = std::numeric_limits<std::uint32_t>::max();
        desc.StencilWriteMask = std::numeric_limits<std::uint32_t>::max();
        DepthStencilOperation defaultOperation = {
            StencilOperation::Keep, StencilOperation::Keep,
            StencilOperation::Keep, ComparisonFunction::Always
        };
        desc.ClockwiseFace = defaultOperation;
        desc.CounterClockwiseFace = defaultOperation;
        return desc;
    }
};

} // namespace Pomdog

#endif // POMDOG_DEPTHSTENCILDESCRIPTION_E14C5D77_HPP
