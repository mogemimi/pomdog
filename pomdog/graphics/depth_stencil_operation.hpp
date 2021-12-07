// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/comparison_function.hpp"
#include "pomdog/graphics/stencil_operation.hpp"

namespace Pomdog {

struct DepthStencilOperation final {
    StencilOperation StencilDepthBufferFail;
    StencilOperation StencilFail;
    StencilOperation StencilPass;
    ComparisonFunction StencilFunction;
};

} // namespace Pomdog
