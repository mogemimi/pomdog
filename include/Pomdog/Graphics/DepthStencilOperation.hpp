// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DEPTHSTENCILOPERATION_5B7A3783_7AEB_44B7_A592_638B9FA4ED05_HPP
#define POMDOG_DEPTHSTENCILOPERATION_5B7A3783_7AEB_44B7_A592_638B9FA4ED05_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

#endif // !defined(POMDOG_DEPTHSTENCILOPERATION_5B7A3783_7AEB_44B7_A592_638B9FA4ED05_HPP)
