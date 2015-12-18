// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "ComparisonFunction.hpp"
#include "StencilOperation.hpp"

namespace Pomdog {

struct DepthStencilOperation {
    StencilOperation StencilDepthBufferFail;
    StencilOperation StencilFail;
    StencilOperation StencilPass;
    ComparisonFunction StencilFunction;
};

} // namespace Pomdog
