// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/stencil_operation.h"

namespace pomdog::gpu {

struct DepthStencilOperation final {
    StencilOperation stencilDepthBufferFail;
    StencilOperation stencilFail;
    StencilOperation stencilPass;
    ComparisonFunction stencilFunction;
};

} // namespace pomdog::gpu
