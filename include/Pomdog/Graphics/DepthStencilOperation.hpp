// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DEPTHSTENCILOPERATION_5B7A3783_HPP
#define POMDOG_DEPTHSTENCILOPERATION_5B7A3783_HPP

#include "ComparisonFunction.hpp"
#include "StencilOperation.hpp"

namespace Pomdog {

class DepthStencilOperation {
public:
    StencilOperation StencilDepthBufferFail = StencilOperation::Keep;

    StencilOperation StencilFail = StencilOperation::Keep;

    StencilOperation StencilPass = StencilOperation::Keep;

    ComparisonFunction StencilFunction = ComparisonFunction::Always;
};

}// namespace Pomdog

#endif // POMDOG_DEPTHSTENCILOPERATION_5B7A3783_HPP
