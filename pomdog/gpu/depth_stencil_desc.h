// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/depth_stencil_operation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// DepthStencilDesc describes the depth and stencil test state for a graphics pipeline.
struct POMDOG_EXPORT DepthStencilDesc final {
    /// Stencil operations applied to clockwise-wound (front-facing) polygons.
    DepthStencilOperation clockwiseFace = {};

    /// Stencil operations applied to counter-clockwise-wound (back-facing) polygons.
    DepthStencilOperation counterClockwiseFace = {};

    /// Reference value for stencil comparison and replace operations.
    i32 referenceStencil = 0;

    /// Bitmask applied to both the reference and stored stencil value before comparison.
    u32 stencilMask = std::numeric_limits<u32>::max();

    /// Bitmask applied to the stencil value written by stencil operations.
    u32 stencilWriteMask = std::numeric_limits<u32>::max();

    /// Comparison function used for the depth test.
    ComparisonFunction depthBufferFunction = ComparisonFunction::LessEqual;

    /// Enables depth testing against the depth buffer when true.
    bool depthBufferEnable = false;

    /// Enables writing the tested depth value to the depth buffer when true.
    bool depthBufferWriteEnable = false;

    /// Enables stencil testing when true.
    bool stencilEnable = false;

    /// Returns a depth-stencil state equivalent to `createReadWriteDepth()`.
    [[nodiscard]] static DepthStencilDesc
    createDefault() noexcept;

    /// Returns a depth-stencil state with depth testing and writing enabled using `ComparisonFunction::LessEqual`.
    [[nodiscard]] static DepthStencilDesc
    createReadWriteDepth() noexcept;

    /// Returns a depth-stencil state with depth testing enabled but depth writing disabled.
    [[nodiscard]] static DepthStencilDesc
    createReadOnlyDepth() noexcept;

    /// Returns a depth-stencil state with both depth testing and stencil testing disabled.
    [[nodiscard]] static DepthStencilDesc
    createNone() noexcept;
};

} // namespace pomdog::gpu
