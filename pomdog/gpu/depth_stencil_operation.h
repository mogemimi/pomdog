// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/stencil_operation.h"

namespace pomdog::gpu {

/// DepthStencilOperation describes the stencil operation actions for one face of a polygon.
struct DepthStencilOperation final {
    /// Action taken when the stencil test passes but the depth test fails.
    StencilOperation stencilDepthBufferFail = StencilOperation::Keep;

    /// Action taken when the stencil test fails.
    StencilOperation stencilFail = StencilOperation::Keep;

    /// Action taken when both the stencil and depth tests pass.
    StencilOperation stencilPass = StencilOperation::Keep;

    /// Comparison function for the stencil test.
    ComparisonFunction stencilFunction = ComparisonFunction::Always;
};

} // namespace pomdog::gpu
