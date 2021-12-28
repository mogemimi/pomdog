// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/comparison_function.h"
#include "pomdog/gpu/stencil_operation.h"

namespace pomdog {

struct DepthStencilOperation final {
    StencilOperation StencilDepthBufferFail;
    StencilOperation StencilFail;
    StencilOperation StencilPass;
    ComparisonFunction StencilFunction;
};

} // namespace pomdog
