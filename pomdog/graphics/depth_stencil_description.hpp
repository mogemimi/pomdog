// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/comparison_function.hpp"
#include "pomdog/graphics/depth_stencil_operation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

struct POMDOG_EXPORT DepthStencilDescription final {
    DepthStencilOperation ClockwiseFace;
    DepthStencilOperation CounterClockwiseFace;
    std::int32_t ReferenceStencil;
    std::uint32_t StencilMask;
    std::uint32_t StencilWriteMask;
    ComparisonFunction DepthBufferFunction;
    bool DepthBufferEnable;
    bool DepthBufferWriteEnable;
    bool StencilEnable;

    static DepthStencilDescription CreateDefault() noexcept
    {
        return CreateReadWriteDepth();
    }

    static DepthStencilDescription CreateReadWriteDepth() noexcept
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
            StencilOperation::Keep,
            StencilOperation::Keep,
            StencilOperation::Keep,
            ComparisonFunction::Always,
        };
        desc.ClockwiseFace = defaultOperation;
        desc.CounterClockwiseFace = defaultOperation;
        return desc;
    }

    static DepthStencilDescription CreateReadOnlyDepth() noexcept
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
            StencilOperation::Keep,
            StencilOperation::Keep,
            StencilOperation::Keep,
            ComparisonFunction::Always,
        };
        desc.ClockwiseFace = defaultOperation;
        desc.CounterClockwiseFace = defaultOperation;
        return desc;
    }

    static DepthStencilDescription CreateNone() noexcept
    {
        DepthStencilDescription desc;
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
};

} // namespace Pomdog
