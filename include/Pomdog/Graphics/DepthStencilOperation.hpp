// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ComparisonFunction.hpp"
#include "Pomdog/Graphics/StencilOperation.hpp"

namespace Pomdog {

struct DepthStencilOperation {
    StencilOperation StencilDepthBufferFail;
    StencilOperation StencilFail;
    StencilOperation StencilPass;
    ComparisonFunction StencilFunction;
};

} // namespace Pomdog
