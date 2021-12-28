// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/comparison_function.h"
#include "pomdog/graphics/stencil_operation.h"

namespace pomdog {

struct DepthStencilOperation final {
    StencilOperation StencilDepthBufferFail;
    StencilOperation StencilFail;
    StencilOperation StencilPass;
    ComparisonFunction StencilFunction;
};

} // namespace pomdog
